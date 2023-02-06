#include <device/arp.h>
#include <device/ethernet.h>
#include <device/ip.h>
#include <kernlib/assert.h>
#include <kernlib/mbuf.h>
#include <kernlib/stdio.h>

// TODO: 尽量不要使用硬编码
// 52:54:00:12:34:56
static uint8_t local_mac[ETHADDR_LEN] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
// static uint8_t local_mac[ETHADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t broadcast_mac[ETHADDR_LEN] = {0xFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};

void eth_tx(struct mbuf *m, uint16_t ethType)
{
    struct eth_hdr *ethhdr;

    ethhdr = (struct eth_hdr *)mbufpush(m, sizeof(struct eth_hdr));

    // broadcast
    for (int i = 0; i < ETHADDR_LEN; i++) {
        ethhdr->srchost[i] = local_mac[i];
        ethhdr->dsthost[i] = broadcast_mac[i];
    }
    ethhdr->type = htons(ethType);  // 变为小端模式

    kprintf("%s \n", "ethernet tx ready");
    kprintf("buffer_len = %u\n", m->buffer_len);
    kprintf("header_end = 0x%x, all_buf = 0x%x\n", m->header_end, m->all_buf);

    kprintf("mbuf: ");
    char *test = (char *)m->header_end;
    for (int i = 0; i < m->buffer_len; ++i, ++test)
        kprintf("%d ", *(u8 *)test);
    kprintf("\n");

    struct ip_hdr *iphdr;
    iphdr = (struct ip_hdr *)(m->all_buf + sizeof(struct eth_hdr));
    kprintf("%s %u\n", "ttl test", iphdr->ttl);

    int n = e1000_transmit(m);

    mbuffree(m);
    kprintf("%s %d\n", "finished net write", n);
}

void eth_rx(struct mbuf *m)
{
    struct eth_hdr *ethhdr;
    uint16_t        ethtype;

    ethhdr = (struct eth_hdr *)mbufpull(m, sizeof(struct eth_hdr));
    kprintf("receive eth packet\n");

    if (ethhdr == NULL) {
        kprintf("fail in eth \n");
        return;
    }
    ethtype = ntohs(ethhdr->type);
    kprintf("%s %d\n", "ethtype", ethtype);

    if (ethtype == ETHTYPE_IP)
        // e1000 not support ip
        // ip_rx(m);
        panic("ip");
    else if (ethtype == ETHTYPE_ARP)
        arp_rx(m);
    else
        mbuffree(m);

    kprintf("fininsh ethernet\n");
}