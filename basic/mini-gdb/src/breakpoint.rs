use std::io;

use nix::libc::{c_long, pid_t, ptrace, PTRACE_PEEKDATA, PTRACE_POKEDATA};

use crate::Res;

pub struct BreakPoint {
    pub pid: pid_t,
    pub addr: u64,
    pub enabled: bool,
    pub saved_data: u8,
}

impl BreakPoint {
    pub fn new(pid: pid_t, addr: u64) -> BreakPoint {
        BreakPoint {
            pid,
            addr,
            enabled: false,
            saved_data: 0,
        }
    }
    pub fn enable(&mut self) {
        let data = unsafe { ptrace(PTRACE_PEEKDATA, self.pid, self.addr, 0) } as u64;
        self.saved_data = (data & 0xff_u64) as u8;

        let replaced_data = (data & (!0xff_u64)) | 0xcc_u64; // set the low-order byte to 0xcc(int3)
        unsafe { ptrace(PTRACE_POKEDATA, self.pid, self.addr, replaced_data) };
        let err = get_errno();
        if err < 0 {
            panic!("ptrace error: {}", io::Error::from_raw_os_error(err));
        }

        self.enabled = true;
    }
    pub fn disable(&mut self) {
        let data = unsafe { ptrace(PTRACE_PEEKDATA, self.pid, self.addr, 0) } as u64;
        let replaced_data = (data & !0xff) | self.saved_data as u64;
        unsafe { ptrace(PTRACE_POKEDATA, self.pid, self.addr, replaced_data) };
        self.enabled = false;
    }
}

fn get_errno() -> i32 {
    unsafe { nix::libc::__errno_location().read() }
}

fn errno_result(r: c_long) -> Res<()> {
    if r < 0 {
        let errno = get_errno();
        Err(io::Error::from_raw_os_error(errno))?
    } else {
        Ok(())
    }
}