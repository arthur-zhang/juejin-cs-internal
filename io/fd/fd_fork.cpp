#include <iostream>
#include <sys/fcntl.h>
#include <gtest/gtest.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>


const int PORT = 8080;
const int BACKLOG = 5;
using namespace std;

void bind_and_listen(int sockfd) {
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    listen(sockfd, BACKLOG);
}

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 绑定地址和端口, 监听连接
    bind_and_listen(sockfd);
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork child" << std::endl;
        return 1;
    }
    cin.get();
}
