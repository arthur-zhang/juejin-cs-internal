#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // 定义要分配的内存大小（例如，10GB）
    size_t size = 10L * 1024 * 1024 * 1024;

    // 分配内存
    char *buffer = (char *)malloc(size);
    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < size; i += 4096) {
        buffer[i] = 0;
    }

    printf("Allocated and touched %zu bytes of memory. PID: %d\n", size, getpid());
    printf("Press Enter to exit...\n");
    getchar();

    free(buffer);
    return 0;
}