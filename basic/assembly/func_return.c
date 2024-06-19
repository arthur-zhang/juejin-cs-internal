
long foo() {
    return 100;
}

struct Foo {
    long x[20];
};

struct Foo xyz() {
    struct Foo foo;
    return foo;
}

void abc(struct Foo *foo) {
}

void main_AAA() {
    struct Foo result = xyz();
}

void main_BBB() {
    struct Foo foo;
    abc(&foo);
}