use std::ffi::CString;
use std::fs::File;
use std::ptr::null;

use nix::libc::{c_char, fork, ptrace, PTRACE_TRACEME};

mod debugger;
mod breakpoint;
mod regs;

pub type Res<T> = anyhow::Result<T>;

fn main() -> Res<()> {
    println!("Hello, world!");
    let pid = unsafe { fork() };
    if pid < 0 {
        eprintln!("could not fork");
        std::process::exit(2);
    }
    let prog = "/data/dev/ya/rgdb/a.out";
    if pid == 0 {
        println!("in child");
        unsafe { nix::libc::personality(nix::libc::ADDR_NO_RANDOMIZE as u64) };

        let ret = unsafe { ptrace(PTRACE_TRACEME, 0, 0, 0) };
        if ret < 0 {
            eprintln!("ptrace error");
            std::process::exit(1);
        }
        let prog = CString::new(prog.to_string()).unwrap();
        let null = null::<c_char>();
        let argv = vec![prog.as_ptr(), null];
        unsafe { nix::libc::execv(prog.as_ptr(), argv.as_ptr()); };
    } else {
        println!("in parent process, start debug {}", pid);
        let f = File::open(&prog).unwrap();
        let mmap = unsafe { memmap::Mmap::map(&f).unwrap() };
        let file = object::File::parse(&*mmap).unwrap();
        let mut debugger = debugger::Debugger::new(file, prog.to_string(), pid);
        debugger.run()?;
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use std::fs::File;

    #[test]
    fn test_dwarf() {
        let file_path = "/data/dev/ya/rgdb/a.out";
        // let file = File::open(file_path).unwrap();
        // let mmap = unsafe { memmap::Mmap::map(&file).unwrap() };
        // let object = object::File::parse(&*mmap).unwrap();
        //
        // println!("object: {:?}", object.kind());
        // println!("object: {:?}", object.endianness());
        // for sec in object.sections() {
        //     println!("sec: {:?}", sec);
        // }
        // // F: FnMut(SectionId) -> core::result::Result<T, E>,
        //
        // let load_fn = |id: SectionId| -> Result::<EndianRcSlice<LittleEndian>, gimli::Error>{
        //     let data = object.section_by_name(id.name())
        //         .and_then(|sec| sec.uncompressed_data().ok())
        //         .unwrap_or(Cow::Borrowed(&[][..]));
        //     Ok(EndianRcSlice::new(Rc::from(&*data), LittleEndian))
        // };
        // let dwarf = Dwarf::load(&load_fn).unwrap();
        // let ctx = addr2line::Context::from_dwarf(dwarf).unwrap();
        //
        // let a  =  ctx.find_location(0x619).unwrap().unwrap();


        let file = File::open(&file_path).unwrap();
        let mmap = unsafe { memmap::Mmap::map(&file).unwrap() };
        let file = object::File::parse(&*mmap).unwrap();

        let ctx = addr2line::Context::new(&file).unwrap();
        let loc = ctx.find_location(0x619).unwrap().unwrap();
        println!("line :  {:?}", loc.line);
        println!("file:   {:?}", loc.file);
        println!("colomn: {:?}", loc.column);
    }
}