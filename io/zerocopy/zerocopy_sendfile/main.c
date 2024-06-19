#include <strings.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>        /* ANSI C header file */
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#define    LISTENQ        1024    /* 2nd argument to listen() */
#define    BUFER_SIZE    64        /* size of buffer used by setbuf */

void
err_quit(const char *fmt) {
    printf("%s", fmt);
    exit(1);
}

int main() {
    printf("Hello, World!\n");

    struct sockaddr_in serv_addr;

    int listen_fd = 0;
    // socket(int domain, int type, int protocol);
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        exit(1);
    }
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(34567);

    int optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval)) == -1) {
        close(listen_fd);
        err_quit("set socket opt failed");
    }
    if (bind(listen_fd, &serv_addr, sizeof(serv_addr)) < 0)
        err_quit("bind failed");

    if (listen(listen_fd, LISTENQ) < 0)
        err_quit("listen failed");

    struct sockaddr_in client_addr;

    int connect_fd = 0;
    int fd = open("./index.html", O_RDWR);

    if (fd < 0) err_quit("error open file");


    for (;;) {

        connect_fd = accept(listen_fd, &serv_addr, &client_addr);
        if (connect_fd < 0) err_quit("accept failed");

        struct stat stat_buf;
        fstat(fd, &stat_buf);

        off_t offset = 0;

        int cnt = 0;
        if ((cnt = sendfile(connect_fd, fd, &offset, stat_buf.st_size)) < 0) {
            err_quit("send file failed");
        }
        close(connect_fd);
    }
}



