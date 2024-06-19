#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>


int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return -1;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

// IP 数据包头结构定义
struct ip_header {
    unsigned char ihl: 4;
    unsigned char version: 4;
    unsigned char tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char ttl;
    unsigned char protocol;
    unsigned short check;
    unsigned int saddr;
    unsigned int daddr;
};

void print_ip_header(struct ip_header *ip) {
    printf("IP Header\n");
    printf("   |-Version        : %d\n", ip->version);
    printf("   |-Header Length  : %d Bytes\n", ((ip->ihl) * 4));
    printf("   |-Type Of Service: %d\n", ip->tos);
    printf("   |-Total Length   : %d Bytes\n", ip->tot_len);
    printf("   |-Identification : %d\n", ip->id);
    printf("   |-TTL            : %d\n", ip->ttl);
    printf("   |-Protocol       : %d\n", ip->protocol);
    printf("   |-Checksum       : %d\n", ip->check);
    printf("   |-Source IP      : %d.%d.%d.%d\n", ip->saddr & 0xFF, (ip->saddr >> 8) & 0xFF, (ip->saddr >> 16) & 0xFF,
           (ip->saddr >> 24) & 0xFF);
    printf("   |-Destination IP : %d.%d.%d.%d\n", ip->daddr & 0xFF, (ip->daddr >> 8) & 0xFF, (ip->daddr >> 16) & 0xFF,
           (ip->daddr >> 24) & 0xFF);
}

void print_payload(unsigned char *packet, int offset, int size) {
    printf("Data Payload:\n\t");
    for (int i = offset; i < size; i++) {
        printf("%02X ", packet[i]);
        if (i % 8 == ((offset - 1) % 8)) {
            printf("\n\t");
        }
    }
}

int main() {
    char tun_name[IFNAMSIZ];
    int tun_fd, nread;
    unsigned char buffer[1500];

    strcpy(tun_name, "tun1");
    tun_fd = tun_alloc(tun_name);

    if (tun_fd < 0) {
        perror("Allocating interface");
        exit(1);
    }

    printf("TUN/TAP device %s opened\n", tun_name);

    while (1) {
        nread = read(tun_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            perror("Reading from interface");
            close(tun_fd);
            exit(1);
        }
        print_ip_header((struct ip_header *) buffer);
        print_payload(buffer, 20, nread);
    }

    return 0;
}