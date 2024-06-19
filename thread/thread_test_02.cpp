#include <iostream>
#include <pthread.h>

void *thread_func(void *args) {
    int *num = (int *) args;
    std::cout << "hello in new thread, arg: " << *num << std::endl;
    return nullptr;
}

int main() {
    pthread_t t1;
    int num = 100;
    int ret = pthread_create(&t1, nullptr, thread_func, &num);
    if (ret != 0) {
        std::cerr << "Error creating thread: " << ret << std::endl;
        return 1;
    }
    ret = pthread_join(t1, nullptr);
    if (ret != 0) {
        std::cerr << "Error joining thread: " << ret << std::endl;
        return 1;
    }
    return 0;
}