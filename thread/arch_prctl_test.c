#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <asm/prctl.h>        /* Definition of ARCH_* constants */

#ifndef __NR_arch_prctl
#define __NR_arch_prctl 158
#endif

// 定义arch_prctl系统调用的包装函数
int arch_prctl(int code, unsigned long *addr) {
    return syscall(__NR_arch_prctl, code, addr);
}

int main() {
    unsigned long fs_base;
    int ret;

    // 使用arch_prctl获取FS基址寄存器的值
    ret = arch_prctl(ARCH_GET_FS, &fs_base);

    if (ret == 0) {
        printf("FS base: 0x%lx\n", fs_base);
    } else {
        perror("arch_prctl");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}