#include <iostream>
#include <sys/fcntl.h>
#include <gtest/gtest.h>

using namespace std;
namespace fd {

    TEST(fd01, fd01) {
        string file_name = "/tmp/test.txt";
        int fd = open(file_name.c_str(), O_RDWR);
        if (fd == -1) {
            // 打开文件失败
            std::cout << "open failed: " << file_name << std::endl;
        } else {
            // 可以使用此 fd 进行文件读写
            cout << "file fd: " << fd << endl;
        }
    }

}
