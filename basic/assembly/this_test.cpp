#include <iostream>

class Foo {

private:
    int id;
public :
    Foo() {
        this->id = 0;
    }

    Foo* bar() {
        return this;
    }
};

Foo* bar(Foo* this_) {
    return this_;
}

long xyz(long ptr){
    return ptr;
}

int main() {
    std::cout << "hello" << std::endl;
    Foo foo;
    foo.bar();
}