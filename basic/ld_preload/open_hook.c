#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>

int (*orig_open)(const char *, int, ...);

int open(const char *filename, int flags, ...) {
    orig_open = dlsym(RTLD_NEXT, "open");
    int mode = 0; // todo: get mode from varargs
    int result = orig_open(filename, flags, mode);
    fprintf(stderr, "[audit] open %s, fd: %d\n", filename, result);
    return result;
}