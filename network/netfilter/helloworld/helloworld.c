#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/icmp.h>

// 定义钩子函数
static unsigned int hook_func(
        void *priv,
        struct sk_buff *skb, // 指向 sk_buff 的指针
        const struct nf_hook_state *state
) {
    struct iphdr *iph; // 指向 IP 头的指针
    struct icmphdr *icmph; // 指向 ICMP 头的指针

    if (!skb)
        return NF_ACCEPT;

    // 获取 IP 头部,如果获取失败则返回NF_ACCEPT
    iph = ip_hdr(skb);

    if (!iph)
        return NF_ACCEPT;

    // 检查是否为 ICMP 协议，如果是则获取 ICMP 头部并打印 Hello World
    if (iph->protocol == IPPROTO_ICMP) {
        icmph = icmp_hdr(skb);

        printk(KERN_INFO "Hello World, ICMP Packet Received\n");
    }

    // 无论数据包是否为ICMP协议,最后都返回NF_ACCEPT,允许数据包继续传输。
    return NF_ACCEPT;
}

// 定义钩子操作结构
static struct nf_hook_ops nfho = {
        .hook = hook_func, // 定义钩子函数
        .pf = PF_INET, // 表示 IPv4 协议
        .hooknum = NF_INET_PRE_ROUTING, // 在 PRE_ROUTING 阶段处理数据包
        .priority = NF_IP_PRI_FIRST,  // 最高优先级
};

// 模块初始化函数
static int __init hook_init(void) {
    nf_register_net_hook(&init_net, &nfho); // 注册钩子函数
    return 0;
}

// 模块卸载函数
static void __exit hook_exit(void) {
    nf_unregister_net_hook(&init_net, &nfho); // 注销钩子函数
}

module_init(hook_init);
module_exit(hook_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arthur.Zhang");
MODULE_DESCRIPTION("A simple module to print Hello World for ICMP packets");