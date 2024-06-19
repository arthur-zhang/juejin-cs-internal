#include <iostream>
#include <thread>
#include <atomic>

extern "C" bool compare_and_swap(std::atomic<int> *x, int a, int b);

std::atomic<int> counter{0};

void increment() {
    for (int i = 0; i < 1000000; ++i) {
        int old_val;
        int new_val;
        do {
            old_val = counter.load();
            new_val = old_val + 1;
        } while (!compare_and_swap(&counter, old_val, new_val));
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << std::endl;
    return 0;
}
