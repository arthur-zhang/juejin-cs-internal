#include <atomic>

struct node {
    int data;
    node *next;

    node(int x) : data(x), next(nullptr) {}
};

class Stack {
private:
    std::atomic<node *> top;
public:
    void push(int val) {
        node *newTop = new node(val);
        node *oldTop = nullptr;
        do {
            oldTop = top.load();
            newTop->next = oldTop;
        } while (!top.compare_and_swap(oldTop, newTop));
    }

    bool pop(int &val) {
        node *oldTop = nullptr;
        do {
            oldTop = top.load();
            if (oldTop == nullptr) {
                return false;
            }
        } while (!top.compare_and_swap(oldTop, oldTop->next));
        val = oldTop->data;
        return true;
    }
};
