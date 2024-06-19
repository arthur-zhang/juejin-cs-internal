//
// Created by arthur on 2022/5/25.
//

#include <iostream>
#include <sys/fcntl.h>
#include <unistd.h>

const int SIZE = 10;
namespace fd {

    int main2() {

        std::cout << "hello" << std::endl;
        int fds[SIZE];
        for (int i = 0; i < SIZE; ++i) {
            std::string file_name = "/tmp/test.txt" + std::to_string(i);
            int fd = ::open(file_name.c_str(), O_CREAT, 0644);
            fds[i] = fd;
            std::cout << "fd: " << fd << std::endl;
        }
        std::cout << "close fd" << std::endl;
        for (int i = 0; i < SIZE; i += 2) {
            ::close(fds[i]);
        }
        std::cout << "start open fd..." << std::endl;
        for (int i = SIZE; i < SIZE * 2; ++i) {
            std::string file_name = "/tmp/test.txt" + std::to_string(i);
            int fd = ::open(file_name.c_str(), O_CREAT, 0644);
            std::cout << "fd is :" << fd << std::endl;
        }
    }
}
