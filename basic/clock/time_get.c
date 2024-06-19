#include <time.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	unsigned long *p = (unsigned long *) 0x7ffff7ff70a8;
    time_t t = time(0);
    printf("time by func: %ld\n", t);
	printf("time by addr: %lu\n", *p);
}
