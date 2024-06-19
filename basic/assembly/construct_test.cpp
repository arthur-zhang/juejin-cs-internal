#include <iostream>

class Foo {
public :
    Foo() {
        x = 100;
    }

    ~Foo() {
        std::cout << "~Foo() called" << std::endl;
    }

private:
    int x;
};

class Bar {

public:
    Bar() {
    }

    ~Bar() {
        std::cout << "~Bar() called" << std::endl;
    }
};

int main() {
    {
        Foo foo;
    }

    Bar *b = new Bar();
    delete b;
    return 0;
}