#include <device/ethernet.h>
#include <kernlib/assert.h>
#include <kernlib/stdio.h>

// TODO: 尽量不要使用硬编码
// 52:54:00:12:34:56
static uint8_t local_mac[ETHADDR_LEN] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };
// static uint8_t local_mac[ETHADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t broadcast_mac[ETHADDR_LEN] = { 0xFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF };

void eth_tx(struct mbuf *m, uint16_t ethType) {
    struct eth_hdr *ethhdr;

    ethhdr = (struct eth_hdr*)mbufpush(m, sizeof(struct eth_hdr));

    // broadcast
    for (int i = 0; i < ETHADDR_LEN; i++) {
        ethhdr->srchost[i] = local_mac[i];
        ethhdr->dsthost[i] = broadcast_mac[i];
    }
    ethhdr->type = htons(ethType); // 变为小端模式

    kprintf("%s \n", "ethernet tx ready");
    kprintf("%s %d\n", "len of buf", m->len);
    kprintf("%s %d\n", "head", m->head - m->buf);

    struct ip_hdr *iphdr;
    iphdr = (struct ip_hdr*)(m->buf + 14);
    kprintf("%s %d\n", "ttl test", iphdr->ttl);
    
    int n = e1000_transmit(m);

    mbuffree(m);
    kprintf("%s %d\n", "finished net write", n);
}

void eth_rx(struct mbuf* m) {
    struct eth_hdr *ethhdr;
    uint16_t ethtype;

    ethhdr = (struct eth_hdr*) mbufpull(m, sizeof(struct eth_hdr));
    kprintf("receive eth packet\n");

    if (ethhdr == NULL) {
        printf("fail in eth \n");
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