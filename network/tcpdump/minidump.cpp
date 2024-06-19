//
// Created by arthur on 2024/4/10.
//
#include <iostream>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>

// 以太网帧头部结构体
struct EthernetHeader {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* 目的以太网地址 */
    u_char ether_shost[ETHER_ADDR_LEN]; /* 源以太网地址 */
    u_short ether_type; /* 以太网类型 */
};
// IPv4 头部
struct IPv4Header {
    u_char ip_vhl;      /* 版本号和头部长度(in words) */
    u_char ip_tos;      /* 服务类型 */
    u_short ip_len;     /* 总长度 */
    u_short ip_id;      /* 标识 */
    u_short ip_off;     /* 片偏移 */
    u_char ip_ttl;      /* 生存时间 */
    u_char ip_p;        /* 协议 */
    u_short ip_sum;     /* 校验和 */
    struct in_addr ip_src; /* 源地址 */
    struct in_addr ip_dst; /* 目的地址 */
};

// TCP 头部
struct TCPHeader {
    u_short th_sport;   /* 源端口号 */
    u_short th_dport;   /* 目的端口号 */
    u_int th_seq;       /* 序列号 */
    u_int th_ack;       /* 确认号 */
    u_char th_offset;   /* 数据偏移 */
    u_char th_flags;    /* TCP flags */
    u_short th_win;     /* 窗口大小 */
    u_short th_sum;     /* 校验和 */
    u_short th_urp;     /* 紧急数据偏移量 */
};

std::string tcp_flags(u_char flags) {
    std::string result;
    if (flags & TH_FIN) {
        result += "F";
    }
    if (flags & TH_SYN) {
        result += "S";
    }
    if (flags & TH_RST) {
        result += "R";
    }
    if (flags & TH_PUSH) {
        result += "P";
    }
    if (flags & TH_URG) {
        result += "U ";
    }
    if (flags & TH_ACK) {
        result += ".";
    }
    return result;
}

int main() {
    int sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_fd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    while (true) {
        char buf[2048];
        struct sockaddr addr;
        socklen_t addr_len = sizeof(addr);
//        ssize_t n = recvfrom(sock_fd, &buf, sizeof(buf), 0, (struct sockaddr *) &addr, &addr_len);
        ssize_t n = recvfrom(sock_fd, &buf, sizeof(buf), 0, nullptr, nullptr);
        if (n < 0) {
            std::cerr << "Failed to receive data" << std::endl;
            continue;
        }
        auto *eth_header = (struct EthernetHeader *) buf;
        ushort ether_type = ntohs(eth_header->ether_type);
        if (ether_type == ETHERTYPE_IP) {
            auto *ipv4_header = (IPv4Header *) (buf + sizeof(EthernetHeader));
            if (ipv4_header->ip_p == IPPROTO_TCP) {
                auto *tcp_header = (TCPHeader *) (buf + sizeof(EthernetHeader) + sizeof(IPv4Header));
                u_char tcp_header_length = (tcp_header->th_offset >> 4) * 4;
                if (ntohs(tcp_header->th_dport) == 8080 || ntohs(tcp_header->th_sport) == 8080) {
                    uint16_t tcp_header_len = (tcp_header->th_offset >> 4) * 4;
                    std::cout << inet_ntoa(ipv4_header->ip_src) << "." << ntohs(tcp_header->th_sport) << " > "
                              << inet_ntoa(ipv4_header->ip_dst) << "." << ntohs(tcp_header->th_dport) << " "
                              << "Flags [" << tcp_flags(tcp_header->th_flags) << "], "
                              << "seq " << ntohl(tcp_header->th_seq) << ", ack " << ntohl(tcp_header->th_ack) << ", "
                              << "win " << ntohs(tcp_header->th_win) << ", "
                              << "length "
                              << n - sizeof(EthernetHeader) - sizeof(IPv4Header) - tcp_header_length
                              << std::endl;
                }
            }
        }
    }
}

