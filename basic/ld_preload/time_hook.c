#define _GNU_SOURCE
#include <time.h>
#include <dlfcn.h>

time_t time(time_t *t) {
    static time_t (*real_time)(time_t *) = NULL;
    if (!real_time) {
        real_time = (time_t (*)(time_t *))dlsym(RTLD_NEXT, "time");
        if (!real_time) {
            return -1;
        }
    }
    time_t fixed_time = 42; // 固定时间值
    if (t) {
        *t = fixed_time;
    }
    return fixed_time;
}