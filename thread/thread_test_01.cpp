#include <iostream>
#include <pthread.h>

void *thread_func(void *args) {
    std::cout << "hello in new thread" << std::endl;
    return nullptr;
}

int main() {
    pthread_t thread;
    int ret = pthread_create(&thread, nullptr, thread_func, nullptr);
    if (ret != 0) {
        std::cerr << "Error creating thread: " << ret << std::endl;
        return 1;
    }
    ret = pthread_join(thread, nullptr);
    if (ret != 0) {
        std::cerr << "Error joining thread: " << ret << std::endl;
        return 1;
    }
    return 0;
}