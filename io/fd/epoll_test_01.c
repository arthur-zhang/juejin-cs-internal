#include <unistd.h>

#ifdef linux
#include <sys/epoll.h>
#endif

#define MAX_EVENTS 10

int main() {
    int epfd;
    if ((epfd = epoll_create(0)) < 0) {
        exit(1);
    }
    int sock_fd = create_sock_fd();//
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = sock_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
        exit(1);
    }

    struct epoll_event events[MAX_EVENTS];
    int n;
    while (1) {
        n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n < 0) {
            exit(1);
        }
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == sock_fd) {
                // do something
                handle_event(events[i]);
            }
        }
    }
    close(epfd);
}