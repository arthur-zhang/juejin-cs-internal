#include <stdio.h>
#include <pthread.h>

// 定义全局变量
//__thread int g_foo = 0x12345678;
//__thread int g_bar = 0x55555555;

int g_foo = 0x12345678;
int g_bar = 0x55555555;

void *thread_func(void *arg) {
    g_foo += 1;
    g_bar += 2;
    printf("in new thread, g_foo[%p]=0x%x, g_bar[%p]=0x%x\n", &g_foo, g_foo, &g_bar, g_bar);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("in main thread, g_foo[%p]=0x%x, g_bar[%p]=0x%x\n", &g_foo, g_foo, &g_bar, g_bar);
    return 0;
}