#include <iostream>
#include <atomic>

void foo(std::atomic<int> &n) {
    int old_n = n.fetch_add(1);
    std::cout << old_n << std::endl;
}

void bar() {
    std::atomic<int> n(10);
    int expected_value = 10;
    int update_value = 11;
    bool ok = n.compare_exchange_strong(expected_value, update_value);
    std::cout << ok << std::endl;
}

int main() {
    std::atomic<int> n(0);
    foo(n);

    return 0;
}
