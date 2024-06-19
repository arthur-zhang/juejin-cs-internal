#include <iostream>

class Base {
public:
    Base() {
        this->x = 100;
    }

    ~Base() {
        std::cout << "~Base() called" << std::endl;
    }

private:
    long x;
};

class Foo : public Base {

public:
    Foo() {
        this->y = 200;
    }

    ~Foo() {
        std::cout << "~Foo() called" << std::endl;
    }

    void print() {
        std::cout << "hello" << this->y << std::endl;
    }

private:
    long y;
};

int main() {
    Foo foo;
    foo.print();
}