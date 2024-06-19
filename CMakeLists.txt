cmake_minimum_required(VERSION 3.22)
project(os_course)

set(CMAKE_CXX_STANDARD 20)
enable_testing()

#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})
include_directories(/Users/arthur/CLionProjects/juejin_os_course/include)


#file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true io/fd/*.cpp binarysearch/*.cpp leetcode/*.cpp)
set(SOURCES ${SOURCES}
#        io/fd/fd_fork.cpp
#        thread/thread_test_02.cpp
#        io/fuse/fuse_test_01.cpp
#        io/fuse/fuse_test_02.cpp
#        io/fuse/fuse_test_03.cpp
#        include/fuse_common.h thread/tls_test_01.c thread/tls_test_02.c thread/two_thread.c thread/arch_prctl_test.c
#        thread/cas.cpp
#        io/fd/dup_test01.c
#        io/fd/fd_fork_test_01.c
#        io/fd/epoll_test_01.c
#        io/fd/intrusive_lists_test.c
#        cpu/ucontext_test.c
#        cpu/ucontext_test_2.c
        cpu/my_coroutine.c
        network/tuntap/tuntap_test.c
        network/netfilter/ping-drop/pingdrop.c
        network/tcpdump/minidump.cpp
        thread/atomic/atomic_test_01.cpp
        thread/atomic/cmp_and_swap.cpp
        thread/atomic/test.cpp
        thread/atomic/cmp_and_swap2.cpp
        network/dns/DnsC.c
        network/socket/socket_01.c
        thread/lock-impl/lock_impl.c
        basic/assembly/stack_test.c
        basic/assembly/stack_frame_test.c
        network/route/route_test.cpp
        basic/assembly/virtual_test.cpp
)
message(">>>>>")
message(${SOURCES})

add_executable(os_course ${SOURCES})

target_link_libraries(os_course
#        ${GTEST_BOTH_LIBRARIES}
        pthread
        )

#gtest_discover_tests(os_course)
