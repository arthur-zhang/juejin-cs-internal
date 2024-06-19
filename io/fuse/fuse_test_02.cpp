#include <fuse.h>
#include <cstring>
#include <iostream>

using namespace std;

// Define the file path and content
static const char *file_path = "/data/dev/ya/fuse_test";
static const char *file_content = "Hello from my FUSE filesystem!\n";

// Define the getattr function to handle stat calls
static int getattr_callback(const char *path, struct stat *st) {
    std::cout << "get addr, path: " << path << std::endl;
    memset(st, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    } else if (strcmp(path, file_path) == 0) {
        st->st_mode = S_IFREG | 0444;
        st->st_nlink = 1;
        st->st_size = strlen(file_content);
    } else
        return -ENOENT;

    return 0;
}

// Define the readdir function to handle readdir calls
static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, file_path + 1, NULL, 0);

    return 0;
}

// Define the open function to handle file open calls
static int open_callback(const char *path, struct fuse_file_info *fi) {
    if (strcmp(path, file_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

// Define the read function to handle file read calls
static int read_callback(const char *path, char *buf, size_t size, off_t offset,
                         struct fuse_file_info *fi) {
    size_t len;
    (void) fi;
    if (strcmp(path, file_path) != 0)
        return -ENOENT;

    len = strlen(file_content);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, file_content + offset, size);
    } else
        size = 0;

    return size;
}


int main(int argc, char *argv[]) {
    struct fuse_operations operations = {};

//    operations.getattr = getattr_callback;
//    operations.readdir = readdir_callback;
//    operations.open = open_callback;
//    operations.read = read_callback;
    cout << "FUSE filesystem starting..." << endl;
//    return fuse_main(argc, argv, &operations, nullptr);
    return fuse_main(argc, argv, &operations, nullptr);
}
