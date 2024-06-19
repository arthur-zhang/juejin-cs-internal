void bar(long a, long b) {
}

void foo() {
    long a = 0x1234;
    long b = 0xfefe;
    bar(a, b);
}

int main() {
    foo();
}