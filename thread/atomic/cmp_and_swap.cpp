#include <iostream>
#include <atomic>

extern "C" bool compare_and_swap(std::atomic<int> *x, int a, int b);

int main() {
    std::atomic<int> value(1234);

    int expected = 10;
    int new_value = 100;

    std::cout << "Initial value: " << value.load() << ", expected: " << expected << ", new: " << new_value << std::endl;

    // 尝试将 value 从 10 更新为 100
    bool success = compare_and_swap(&value, expected, new_value);

    if (success) {
        std::cout << "Value updated successfully to " << value.load() << std::endl;
    } else {
        std::cout << "Value was not updated (maybe it was changed by another thread)" << std::endl;
        std::cout << "Current value: " << value.load() << std::endl;
    }

    return 0;
}
