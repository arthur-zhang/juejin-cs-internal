#include <stdio.h>
#include <time.h>


void test_clock(clockid_t clk_id) {
    struct timespec ts;
    clock_gettime(clk_id, &ts);
    printf("ts: %ld.%ld\n", ts.tv_sec, ts.tv_nsec);
}

int main() {
	test_clock(CLOCK_MONOTONIC);
	test_clock(CLOCK_MONOTONIC_COARSE);
    return 0;
}
