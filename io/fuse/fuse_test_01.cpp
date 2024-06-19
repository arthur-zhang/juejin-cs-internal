#include <fuse.h>
#include <cstring>
#include <iostream>

using namespace std;
int main(int argc, char *argv[]) {
    struct fuse_operations operations = {};
    return fuse_main(argc, argv, &operations, nullptr);
}
