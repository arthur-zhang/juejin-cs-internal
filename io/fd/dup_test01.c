#include <unistd.h>
#include <sys/file.h>

int main() {
    int stdout_fd = open("/tmp/output.log", O_RDWR|O_CREAT|O_APPEND, 0600);
    int err_fd = open("/tmp/error.log", O_RDWR|O_CREAT|O_APPEND, 0600);
    dup2(stdout_fd, STDOUT_FILENO);
    dup2(err_fd, STDERR_FILENO);
    sleep(1000);
    return 0;
}