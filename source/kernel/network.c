#include <device/arp.h>
#include <device/ip.h>
#include <kernel/network.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

/**
 * syscall 向量表跳转函数 sys_net_test 的处理函数（内核调用）
 * args 前两个字节 (u16) 用来存大小
 * args 后面的字节用来存内容
 */
void kern_net_test(u8 *args)
{
    u16 len = *(u16 *)args;
    u8 *data = args + 2;

    kern_set_color(CYAN);
    kprintf("This is for net test: %u, %s\n", len, (char *)data);
    kern_set_color(WHITE);

    // struct mbuf *m;

    // m = mbufalloc(sizeof(struct eth_hdr) + len);

    // memmove(hdr->arp_sha, local_mac, ETHADDR_LEN);
    // hdr->arp_sip = htonl(local_ip);

    // memmove(hdr->arp_tha, desmac, ETHADDR_LEN);
    // hdr->arp_tip = htonl(tip);
    
    // eth_tx(m, ETHTYPE_ARP);

    return;
}

void kern_net_arp()
{
    // kprintf("e1000 begin to send pack now...\n");
    uint32_t tip = dst_ip;
    uint8_t  broadcast_mac[ETHADDR_LEN] = {0xFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
    arp_tx(ARP_OP_REQUEST, broadcast_mac, dst_ip);
    return;
}

char *kern_net_receive()
{
    return NULL;
}