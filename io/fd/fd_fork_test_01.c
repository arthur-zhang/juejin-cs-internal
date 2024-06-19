#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    int fd = open("/tmp/output.log", O_RDWR | O_CREAT | O_APPEND, 0600);
    pid_t pid = fork();
    if (pid == 0) {
        printf("in child, pid: %d\n", getpid());
        sleep(100000);
    } else {
        printf("in parent, pid: %d\n", getpid());
        sleep(100000);
    }
}
