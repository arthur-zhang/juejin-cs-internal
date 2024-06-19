#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void *malloc(size_t size) {
    void *(*orig_malloc)(size_t);
    orig_malloc = dlsym(RTLD_NEXT, "malloc");

    if (rand() % 3 == 0) {
        fprintf(stderr, "[fault] malloc(%ld) = NULL\n", size);
        errno = ENOMEM;
        return NULL;
    }

    void* result = orig_malloc(size);
    fprintf(stderr, "[audit] malloc(%ld) = %p\n", size, result);
    return result;
}

//void free(void *ptr) {
//    void (*orig_free)(void*);
//    orig_free = dlsym(RTLD_NEXT, "free");
//
//    fprintf(stderr, "[audit] free(%p)\n", ptr);
//    orig_free(ptr);
//}