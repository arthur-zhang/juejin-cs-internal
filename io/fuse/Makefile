clean:
ifeq ($(shell uname), Darwin)
	find . -perm +111 -type f -exec rm '{}' \;
else
	find .  -maxdepth 1 -type f -executable -exec rm '{}' \;
endif
	rm -f *.o *.S

# -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -lfuse -pthread
FUSE_BUILD_FLAGS = $(shell pkg-config fuse --cflags --libs)
fuse_test_01:clean
	g++ -Wall -std=c++11 -DFUSE_USE_VERSION=26 fuse_test_01.cpp -o fuse_test_01 $(FUSE_BUILD_FLAGS)


fuse_test_02:clean
	g++ -std=c++11 -DFUSE_USE_VERSION=26 fuse_test_02.cpp -o fuse_test_02 $(FUSE_BUILD_FLAGS)

fuse_test_03:clean
	g++ -std=c++11 -DFUSE_USE_VERSION=26 fuse_test_03.cpp -o fuse_test_03 $(FUSE_BUILD_FLAGS)

fuse_test_03_mount:fuse_test_03
	./fuse_test_03 -d /data/dev/ya/fuse_test/
fuse_umount:
	sudo umount /data/dev/ya/fuse_test