use anyhow::anyhow;
use enum_iterator::Sequence;

#[derive(Debug, Sequence)]
pub enum Reg {
    RAX,
    RBX,
    RCX,
    RDX,
    RDI,
    RSI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    RIP,
    RFLAGS,
    CS,
    ORIG_RAX,
    FS_BASE,
    GS_BASE,
    FS,
    GS,
    SS,
    DS,
    ES,
}

impl Reg {
    pub fn name(&self) -> String {
        format!("{:?}", self).to_ascii_uppercase()
    }
}

impl TryFrom<&str> for Reg {
    type Error = anyhow::Error;

    fn try_from(value: &str) -> Result<Self, Self::Error> {
        let value = value.to_ascii_uppercase();
        enum_iterator::all::<Reg>()
            .find(|r| r.name() == value)
            .ok_or_else(|| anyhow!("invalid register name".to_string()))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_regs_for() {
        for x in enum_iterator::all::<Reg>() {
            println!("{}", x.name())
        }
    }
}