#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

static int (*real_rand)(void) = NULL;
int rand(void) {
    if (!real_rand) {
        real_rand = (int (*)(void))dlsym(RTLD_NEXT, "rand");
        if (!real_rand) {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return -1;
        }
    }
    return 42; // 固定随机数
}