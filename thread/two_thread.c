#include <unistd.h>
#include <pthread.h>

void *in_thread(void *unused) {
    while (1) {
        sleep(1);
    }
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, in_thread, NULL);
    pthread_create(&t2, NULL, in_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}