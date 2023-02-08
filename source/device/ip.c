#include <device/ip.h>
#include <kernlib/stdio.h>

void ip_tx_ready(struct mbuf* m) {
    struct ip_hdr *iphdr;
    iphdr = (struct ip_hdr *)mbufpush(m, sizeof(struct ip_hdr));

    if (iphdr == NULL) {
        kprintf("iphdr is NULL\n");
        return;
    }
    iphdr->checksum = 0;
    iphdr->checksum = checksum((unsigned char*)iphdr, sizeof(struct ip_hdr));

    eth_tx(m, ETHTYPE_IP);
}

void ip_tx(struct mbuf *m, uint8_t protocol, uint32_t ip) {
    struct ip_hdr *iphdr;

    kprintf("test %d\n", m->header_end - m->buffer_len);
    iphdr = (struct ip_hdr *)mbufpush(m, sizeof(struct ip_hdr));
    
    if (iphdr == NULL) {
        kprintf("iphdr is NULL\n");
        return;
    }

    memset(iphdr, 0, sizeof(struct ip_hdr));
    iphdr->vhl = (4 << 4) | (20 >> 2);
    iphdr->protocol = protocol;
    // TODO: local ip
    iphdr->src_addr = htonl(local_ip);
    iphdr->dst_addr = htonl(dst_ip);
    iphdr->len = htons(m->buffer_len);
    iphdr->ttl = 64;

    iphdr->checksum = checksum((unsigned char*)iphdr, sizeof(struct ip_hdr));
    kprintf("%d\n", (int)iphdr->checksum);
    kprintf("ip ready for tx\n");

    eth_tx(m, ETHTYPE_IP);
}

void ip_rx(struct mbuf* m) {
    // TODO error process

    struct ip_hdr *iphdr;
    uint16_t len;

    iphdr = (struct ip_hdr*)mbufpull(m, sizeof(struct ip_hdr));

    len = ntohs(iphdr->len) - sizeof(struct ip_hdr);
    kprintf("%s %d\n", "ip protocol", iphdr->protocol);
    if (iphdr->protocol == IPPROTO_UDP)
        udp_rx(m, len, iphdr);
    else if (iphdr->protocol == IPPROTO_ICMP)
        icmp_incoming(m);       
    else 
        mbuffree(m);
    return;
}