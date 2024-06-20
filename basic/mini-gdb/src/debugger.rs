use std::borrow::Cow;
use std::collections::HashMap;
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::mem::MaybeUninit;
use std::ops::Range;
use std::path::Path;
use std::rc::Rc;

use addr2line::LookupContinuation;
use anyhow::anyhow;
use gimli::{AttributeValue, EndianRcSlice, RunTimeEndian};
use nix::libc::{pid_t, ptrace, PTRACE_CONT, PTRACE_GETSIGINFO, PTRACE_PEEKDATA, PTRACE_POKEDATA, PTRACE_SINGLESTEP, siginfo_t, user_regs_struct, waitpid};
use object::{Object, ObjectSection};
use rustyline::DefaultEditor;

use crate::breakpoint::BreakPoint;
use crate::regs::Reg;
use crate::Res;

const SI_KERNEL: i32 = 128;
const TRAP_BRKPT: i32 = 1;
const TRAP_TRACE: i32 = 2;

pub type R = EndianRcSlice<RunTimeEndian>;

pub struct Debugger {
    pub prog_name: String,
    pub pid: pid_t,
    pub break_points: HashMap<u64, BreakPoint>,
    // pub file: object::File<'a>,
    pub dwarf: gimli::Dwarf<R>,
    pub addr2line_ctx: addr2line::Context<R>,
    pub load_addr: u64,
}

impl Debugger {
    pub fn new(file: object::File, prog_name: String, pid: pid_t) -> Debugger {
        let endian = if file.is_little_endian() {
            gimli::RunTimeEndian::Little
        } else {
            gimli::RunTimeEndian::Big
        };

        fn load_section<'data: 'file, 'file, O, Endian>(
            id: gimli::SectionId,
            file: &'file O,
            endian: Endian,
        ) -> anyhow::Result<gimli::EndianRcSlice<Endian>>
            where
                O: object::Object<'data, 'file>,
                Endian: gimli::Endianity,
        {
            use object::ObjectSection;

            let data = file
                .section_by_name(id.name())
                .and_then(|section| section.uncompressed_data().ok())
                .unwrap_or(Cow::Borrowed(&[]));
            Ok(gimli::EndianRcSlice::new(Rc::from(&*data), endian))
        }

        let mut dwarf = gimli::Dwarf::load(|id| load_section(id, &file, endian)).unwrap();
        // let mut dwarf = gimli::Dwarf::load(|id| load_section(id, &file, endian)).unwrap();


        let ctx = addr2line::Context::new(&file).unwrap();


        Debugger {
            prog_name,
            pid,
            break_points: Default::default(),
            // file,
            dwarf: dwarf,
            addr2line_ctx: ctx,
            load_addr: 0,
        }
    }
    pub fn get_function_from_pc(&self, pc: u64) -> Res<Option<Range<u64>>> {
        let (dwarf, unit) = self.addr2line_ctx.find_dwarf_and_unit(pc).skip_all_loads().unwrap();
        // println!("dwarf: {:?}, unit: {:?}", dwarf, unit);
        let mut entries = unit.entries();
        while let Some((_, entry)) = entries.next_dfs()? {
            if entry.tag() != gimli::DW_TAG_subprogram {
                continue;
            }
            // println!("entry: {:?}", entry);

            let low_pc = entry.attr(gimli::DW_AT_low_pc)?.map(|it| {
                if let gimli::AttributeValue::Addr(addr) = it.value() {
                    addr
                } else {
                    panic!("DW_AT_low_pc is not an address");
                }
            });
            let low_pc = low_pc.unwrap_or(0);

            let high_pc = entry.attr(gimli::DW_AT_high_pc)?.map(|it| {
                match it.value() {
                    AttributeValue::Addr(a) => { a }
                    AttributeValue::Sdata(n) => { low_pc + (n as u64) }
                    AttributeValue::Udata(n) => { low_pc + n }
                    _ => { panic!("DW_AT_high_pc is not an address") }
                }
            });
            let high_pc = high_pc.unwrap_or(low_pc + 1);

            let range = low_pc..high_pc;
            println!("range: {:?}", range);
            if range.contains(&pc) {
                println!("found function: {:?}", entry);
                return Ok(Some(range));
            }
        }

        Ok(None)
    }
    pub fn get_line_entry_from_pc(&self, pc: u64) -> Res<Vec<u64>> {
        // let loc = self.addr2line_ctx.find_location(pc).unwrap();
        let range = match self.get_function_from_pc(pc)? {
            None => {
                return Ok(vec![]);
            }
            Some(range) => range
        };
        let iter = self.addr2line_ctx.find_location_range(range.start, range.end)?;
        let locs = iter.map(|(start, end, loc)| start).collect::<Vec<_>>();
        Ok(locs)
    }
    pub fn initialise_load_address(&mut self) -> Res<u64> {
        let f = File::open(format!("/proc/{}/maps", self.pid))?;
        let buf_reader = BufReader::new(f);
        let line = buf_reader.lines().next().transpose()?.ok_or(anyhow!("failed to parse map file"))?;
        let parts = line.split_whitespace().collect::<Vec<_>>();
        let addr = parts[0].split("-").collect::<Vec<_>>();
        let addr = addr[0];
        let addr = u64::from_str_radix(addr, 16)?;
        return Ok(addr);
    }

    pub fn handle_command(&mut self, line: &str) -> Res<()> {
        if line == "c" {
            println!("continue");
            self.continue_execution()?;
        } else if line.starts_with("b ") {
            println!(">>>>>>>>>> add breakpoint");
            let parts = line.split_whitespace().collect::<Vec<_>>();
            let addr = parts[1].trim_start_matches("0x");
            let addr = u64::from_str_radix(addr, 16)?;
            self.set_breakpoint(addr);
            for (addr, bp) in &self.break_points {
                println!("breakpoint: 0x{:x}, enabled: {}", addr, bp.enabled);
            }
        } else if line.starts_with("r ") {
            let parts = line.split_whitespace().collect::<Vec<_>>();
            if parts[1].starts_with("dump") {
                self.dump_registers();
            } else if parts[1].starts_with("read") {
                let reg = parts[2].trim_start_matches("%");
                let reg = Reg::try_from(reg)?;
                let val = get_reg(self.pid, &reg);
                println!("{}: 0x{:x}", reg.name(), val);
            } else if parts[1].starts_with("write") {
                let reg = parts[2].trim_start_matches("%");
                let reg = Reg::try_from(reg)?;
                let val = parts[3].trim_start_matches("0x");
                let val = u64::from_str_radix(val, 16)?;
                set_reg_value(self.pid, reg, val);
            }
        } else if line.starts_with("memory ") {
            let parts = line.split_whitespace().collect::<Vec<_>>();
            if parts[1].starts_with("read") {
                let addr = parts[2].trim_start_matches("0x");
                let addr = u64::from_str_radix(addr, 16)?;
                let data = self.read_memory(addr);
                println!("0x{:x}: 0x{:x}", addr, data);
            } else if parts[1].starts_with("write") {
                let addr = parts[2].trim_start_matches("0x");
                let addr = u64::from_str_radix(addr, 16)?;
                let data = parts[3].trim_start_matches("0x");
                let data = u64::from_str_radix(data, 16)?;
                self.write_memory(addr, data);
            }
        } else if line.starts_with("si") {
            println!("handle single step instruction");
            self.single_step_instruction_with_breakpoint_check()?;
        } else if line.starts_with("step") {
            self.step_in()?;
        }
        Ok(())
    }
    pub fn set_breakpoint(&mut self, addr: u64) {
        let mut bp = BreakPoint::new(self.pid, addr);
        bp.enable();
        self.break_points.insert(addr, bp);
    }
    pub fn read_memory(&self, addr: u64) -> u64 {
        let data = unsafe { ptrace(PTRACE_PEEKDATA, self.pid, addr, 0) } as u64;
        data
    }
    pub fn write_memory(&self, addr: u64, data: u64) {
        unsafe { ptrace(PTRACE_POKEDATA, self.pid, addr, data) };
    }

    pub fn dump_registers(&self) {
        let max_width = enum_iterator::all::<Reg>().map(|reg| reg.name().len()).max().unwrap();
        for reg in enum_iterator::all::<Reg>() {
            let val = get_reg(self.pid, &reg);
            println!("{:max_width$}: 0x{:016x}", reg.name(), val, max_width = max_width);
        }
    }
    pub fn continue_execution(&mut self) -> Res<()> {
        println!("continue execution pc: {:x}", self.get_pc());
        self.step_over_breakpoint()?;
        unsafe { ptrace(PTRACE_CONT, self.pid, 0, 0) };
        self.wait_for_signal()?;
        Ok(())
    }
    fn single_step_instruction(&mut self) -> Res<()> {
        unsafe { ptrace(PTRACE_SINGLESTEP, self.pid, 0, 0); }
        self.wait_for_signal()?;
        Ok(())
    }
    fn single_step_instruction_with_breakpoint_check(&mut self) -> Res<()> {
        let pc = self.get_pc();
        if self.break_points.contains_key(&pc) {
            self.step_over_breakpoint()?;
        } else {
            self.single_step_instruction()?;
        }
        Ok(())
    }
    fn step_over_breakpoint(&mut self) -> Res<()> {
        let pc = self.get_pc();
        if let Some(bp) = self.break_points.get_mut(&pc) {
            bp.disable();
            unsafe { ptrace(PTRACE_SINGLESTEP, self.pid, 0, 0); }
            self.wait_for_signal()?;
        }
        if let Some(bp) = self.break_points.get_mut(&pc) {
            bp.enable();
        }

        Ok(())
    }

    pub fn run(&mut self) -> Res<()> {
        let mut status = 0;
        let mut line_noise = DefaultEditor::new()?;

        self.wait_for_signal()?;

        let addr = self.initialise_load_address()?;
        println!("load address: 0x{:x}", addr);
        self.load_addr = addr;

        loop {
            let line = line_noise.readline(">> ");
            match line {
                Ok(line) => {
                    match line.as_str() {
                        "exit" | "quit" => {
                            break;
                        }
                        _ => {
                            println!("line: {}", line);
                            line_noise.add_history_entry(line.as_str())?;
                            self.handle_command(&line)?;
                        }
                    }
                }
                Err(_) => {
                    break;
                }
            }
        }

        Ok(())
    }
    pub fn get_signal_info(&self) -> siginfo_t {
        let sig = MaybeUninit::<siginfo_t>::uninit();
        let data = sig.as_ptr() as usize;
        unsafe { ptrace(PTRACE_GETSIGINFO, self.pid, 0, data); }
        let sig = unsafe { sig.assume_init() };
        sig
    }
    pub fn wait_for_signal(&self) -> Res<()> {
        println!("start wait for signal....");
        let mut status = 0;
        unsafe { waitpid(self.pid, &mut status, 0); }
        let sig = self.get_signal_info();
        match sig.si_signo {
            nix::libc::SIGTRAP => {
                println!("got SIGTRAP");
                self.handle_sigtrap(&sig);
            }
            nix::libc::SIGSEGV => {
                println!("got SIGSEGV");
            }
            _ => {
                println!("got signal: {}", sig.si_code);
            }
        }
        println!("end  wait for signal....");
        Ok(())
    }

    fn set_pc(&self, pc: u64) {
        set_reg_value(self.pid, Reg::RIP, pc);
    }
    fn get_pc(&self) -> u64 {
        get_reg(self.pid, &Reg::RIP)
    }
    fn offset_load_address(&self, addr: u64) -> u64 {
        addr - self.load_addr
    }
    pub fn print_source_loc(&self, loc: &addr2line::Location, n_lines_context: usize) {
        let path = loc.file.unwrap_or("");
        let line = loc.line.unwrap_or(0) as usize;
        self.print_source(path, line, n_lines_context);
    }
    fn print_source(&self, path: &str, line_no: usize, n_lines_context: usize) {
        let path: &Path = path.as_ref();
        let f = File::open(path).unwrap();
        let f = BufReader::new(f);
        let start = if line_no > n_lines_context { line_no - n_lines_context - 1 } else { 0 };
        let end = line_no + n_lines_context - 1;
        f.lines().skip(start).take(end - start + 1).enumerate().for_each(|(i, line)| {
            let line = line.unwrap_or("".to_string());
            let cursor = if i == n_lines_context { ">" } else { " " };
            println!("{}{}", cursor, line);
        });
    }
    pub fn handle_sigtrap(&self, sig_info: &siginfo_t) {
        println!("handle_sigtrap, si_code: {}", sig_info.si_code);
        match sig_info.si_code {
            SI_KERNEL | TRAP_BRKPT => {
                println!("hit breakpoint at: 0x{:x}", self.get_pc());
                self.set_pc(self.get_pc() - 1);
                let offset = self.offset_load_address(self.get_pc());
                let loc = self.addr2line_ctx.find_location(offset).unwrap();
                if let Some(loc) = loc {
                    self.print_source_loc(&loc, 1);
                }
            }

            _ => {}
        }
    }
    fn step_out(&mut self) -> Res<()> {
        let fp = get_reg(self.pid, &Reg::RBP);
        let return_addr = self.read_memory(fp + 8);
        self.set_breakpoint(return_addr);
        self.continue_execution()?;
        Ok(())
    }
    fn get_offset_pc(&self) -> u64 {
        self.offset_load_address(self.get_pc())
    }
    fn source_for_pc(&self, pc: u64) -> Result<Option<addr2line::Location<'_>>, gimli::Error> {
        self.addr2line_ctx.find_location(pc)
    }
    fn step_in(&mut self) -> Res<()> {
        let offset_pc = self.get_offset_pc();

        let old_loc = self.source_for_pc(offset_pc).unwrap().map(|it| (it.file.unwrap().to_string(), it.line.unwrap()));
        println!("old_loc: {:?}", old_loc);

        loop {
            println!(">>>>>>>step in loop");
            self.single_step_instruction_with_breakpoint_check()?;
            let loc = self.source_for_pc(self.get_offset_pc()).unwrap().map(|it| (it.file.unwrap().to_string(), it.line.unwrap()));

            println!(">>>>>>>step in loop, pc:0x{:x}, loc: {:?}", self.get_pc(), loc);
            if loc.is_none() {
                break;
            }
            if loc != old_loc {
                if let Some((path, line_no)) = loc {
                    self.print_source(&path, line_no as usize, 1);
                }
                break;
            }
        }

        Ok(())
    }
    pub fn step_over(&mut self) -> Res<()> {
        let pc = self.get_offset_pc();
        // let locs = self.get_function_from_pc()
        Ok(())
    }
    pub fn pc_for_function_pred<F>(&self, pred: F) -> Res<Option<u64>> where F: for<'a> Fn(&'a str) -> bool {

        let mut units = self.dwarf.units();
        while let Some(header) = units.next()? {
            let unit = self.dwarf.unit(header)?;
            let mut entries = unit.entries();
            while let Some((_, entry)) = entries.next_dfs()? {
                if entry.tag() != gimli::DW_TAG_subprogram {
                    continue;
                }
                let name = match self.at {  };
            }
        }
        Ok(None)
    }
    pub fn set_breakpoint_at_function(&mut self, name: &str) -> Res<()> {
        Ok(())
    }
}


#[cfg(target_os = "linux")]
fn get_regs(pid: pid_t) -> user_regs_struct {
    let mut regs = MaybeUninit::<user_regs_struct>::uninit();
    let data = regs.as_mut_ptr() as usize;
    unsafe { ptrace(nix::libc::PTRACE_GETREGS, pid, 0, data) };
    unsafe { regs.assume_init() }
}

fn get_reg(pid: pid_t, reg: &Reg) -> u64 {
    let regs = get_regs(pid);
    match reg {
        Reg::RAX => regs.rax,
        Reg::RBX => regs.rbx,
        Reg::RCX => regs.rcx,
        Reg::RDX => regs.rdx,
        Reg::RDI => regs.rdi,
        Reg::RSI => regs.rsi,
        Reg::RBP => regs.rbp,
        Reg::RSP => regs.rsp,
        Reg::R8 => regs.r8,
        Reg::R9 => regs.r9,
        Reg::R10 => regs.r10,
        Reg::R11 => regs.r11,
        Reg::R12 => regs.r12,
        Reg::R13 => regs.r13,
        Reg::R14 => regs.r14,
        Reg::R15 => regs.r15,
        Reg::RIP => regs.rip,
        Reg::RFLAGS => regs.eflags,
        Reg::CS => regs.cs,
        Reg::ORIG_RAX => regs.orig_rax,
        Reg::FS_BASE => regs.fs_base,
        Reg::GS_BASE => regs.gs_base,
        Reg::FS => regs.fs,
        Reg::GS => regs.gs,
        Reg::SS => regs.ss,
        Reg::DS => regs.ds,
        Reg::ES => regs.es,
    }
}

fn set_reg_value(pid: pid_t, r: Reg, value: u64) {
    let mut regs = get_regs(pid);
    match r {
        Reg::RAX => regs.rax = value,
        Reg::RBX => regs.rbx = value,
        Reg::RCX => regs.rcx = value,
        Reg::RDX => regs.rdx = value,
        Reg::RDI => regs.rdi = value,
        Reg::RSI => regs.rsi = value,
        Reg::RBP => regs.rbp = value,
        Reg::RSP => regs.rsp = value,
        Reg::R8 => regs.r8 = value,
        Reg::R9 => regs.r9 = value,
        Reg::R10 => regs.r10 = value,
        Reg::R11 => regs.r11 = value,
        Reg::R12 => regs.r12 = value,
        Reg::R13 => regs.r13 = value,
        Reg::R14 => regs.r14 = value,
        Reg::R15 => regs.r15 = value,
        Reg::RIP => regs.rip = value,
        Reg::RFLAGS => regs.eflags = value,
        Reg::CS => regs.cs = value,
        Reg::ORIG_RAX => regs.orig_rax = value,
        Reg::FS_BASE => regs.fs_base = value,
        Reg::GS_BASE => regs.gs_base = value,
        Reg::FS => regs.fs = value,
        Reg::GS => regs.gs = value,
        Reg::SS => regs.ss = value,
        Reg::DS => regs.ds = value,
        Reg::ES => regs.es = value,
    }
    let data = (&regs) as *const _ as usize;
    unsafe { ptrace(nix::libc::PTRACE_SETREGS, pid, 0, data) };
}

pub fn get_reg_value_from_dwarf_reg(pid: pid_t, reg_num: u64) {}

#[cfg(test)]
mod tests {
    use std::fs::File;

    use super::*;

    #[test]
    fn test_init() {
        let file_path = "/data/dev/ya/rgdb/a.out";
        let file = File::open(&file_path).unwrap();
        let mmap = unsafe { memmap::Mmap::map(&file).unwrap() };
        let file = object::File::parse(&*mmap).unwrap();
        let debugger = Debugger::new(file, file_path.to_string(), 0);
        debugger.get_function_from_pc(0x619);
        let line_entries = debugger.get_line_entry_from_pc(0x619);
        println!("line_entries: {:?}", line_entries);
    }
}