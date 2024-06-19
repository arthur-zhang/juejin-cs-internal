#include <fuse.h>
#include <cstring>
#include <string.h>
#include <iostream>

using namespace std;

// Define the file path and content
static const char *text_file_path = "/a.txt";
static const char *text_file_content = "FUSE filesystem, Hello, World!\n";

static int getattr_callback(const char *path, struct stat *st) {
    std::cout << "[getattr] path: " << path << std::endl;
    memset(st, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
        return 0;
    }
    if (strcmp(path, text_file_path) == 0) {
        st->st_mode = S_IFREG | 0444;
        st->st_nlink = 1;
        st->st_size = strlen(text_file_content);
    }
    return -ENOENT;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info *fi) {
    std::cout << "[readdir] path: " << path << std::endl;

    if (strcmp(path, "/") != 0) return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, text_file_path + 1, NULL, 0);

    return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    std::cout << "[open] path: " << path << std::endl;

    if (strcmp(path, text_file_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
                         struct fuse_file_info *fi) {
    std::cout << "[read] path: " << path << std::endl;
    if (strcmp(path, text_file_path) != 0) return -ENOENT;
    memcpy(buf, text_file_content + offset, size);
    return size;
}


int main(int argc, char *argv[]) {
    struct fuse_operations operations = {};

    operations.getattr = getattr_callback;
    operations.readdir = readdir_callback;
    operations.open = open_callback;
    operations.read = read_callback;
    cout << "FUSE filesystem starting..." << endl;
    return fuse_main(argc, argv, &operations, nullptr);
}
