#include <stdio.h>

void z() {
    long a = 0;
    printf("addr in z:    %p\n", &a);
}

void y() {
    long a = 0;
    printf("addr in y:    %p\n", &a);
    z();
}

void x() {
    long a = 0;
    printf("addr in x:    %p\n", &a);
    y();
}

int main() {
    long a = 0;
    printf("addr in main: %p\n", &a);
    x();
}