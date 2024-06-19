#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static int (*orig_connect)(int, const struct sockaddr*, socklen_t) = NULL;
    if (orig_connect == NULL) {
        orig_connect = dlsym(RTLD_NEXT, "connect");
    }

    fprintf(stderr, "[hook] connect(%d) called\n", sockfd);
    int result = orig_connect(sockfd, addr, addrlen);
    fprintf(stderr, "[hook] connect(%d) returned %d\n", sockfd, result);

    return result;
}