#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);


    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有IP
    server_addr.sin_port = htons(8000); // 端口号

    printf("sockfd: %d\n", sockfd);
    // 绑定socket到指定地址和端口
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    int ret = listen(sockfd, 5);
    if (ret < 0) {
        perror("listen");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int connfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (connfd < 0) {
        perror("accept");
        exit(1);
    }
    printf("client conn fd: %d\n", connfd);
}