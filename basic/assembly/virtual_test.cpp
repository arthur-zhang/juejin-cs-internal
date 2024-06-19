#include <iostream>

class Base {
public:
    virtual long getId_1() { return 0; };

    virtual long getId_2() { return 0; };

    virtual long getId_3() { return 0; };
    long x[10];
};

class Foo : public Base {
public:
    long getId_1() override {
        return 100;
    }
};

class Bar : public Base {
public:
    long getId_1() override {
        return 200;
    }
};

long abc(Base *ptr) {
    long res = ptr->getId_1();;
    res = ptr->getId_2();;
    res = ptr->getId_3();;
    return res;
}

int main() {

    Foo base = Foo();
    abc(&base);
}
