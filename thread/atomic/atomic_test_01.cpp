#include <atomic>
#include<iostream>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }
    int num = atoi(argv[1]);

    std::atomic<int> n;
    std::atomic_init(&n, num);

    int expected = 10;
    int desired = 200;
    n.compare_exchange_strong(expected, desired);
    std::cout << "new:" << n << std::endl;
    return 0;
}