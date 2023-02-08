#include <device/icmp.h>
#include <device/ip.h>
#include <device/socket.h>

void icmp_incoming(struct mbuf* m) {

    struct icmp_hdr* icmphdr = (struct icmp_hdr*)mbufpull(m, sizeof(struct icmp_hdr));

    switch (icmphdr->type) {
    case ICMP_ECHO:
        icmp_reply(m);
        return;
    case ICMP_DST_UNREACHABLE:
        kprintf("haha ICMP_DST_UNREACHABLE GG");
    default:
        kprintf("haha GG");
        break;
    }

    mbuffree(m);
    return;
}

void icmp_reply(struct mbuf* m) {
    kprintf("running\n");
    struct icmp_hdr* icmphdr = (struct icmp_hdr*)mbufpush(m, sizeof(struct icmp_hdr));
    int tmp = m->buffer_len;
    struct ip_hdr *iphdr = (struct ip_hdr *)((char*)icmphdr - sizeof(struct ip_hdr));
    kprintf("pos %d %d \n", m->header_end - m->buffer_len, m->buffer_len);

    icmphdr->type = ICMP_REPLY;
    icmphdr->cksum = 0;
    icmphdr->cksum = checksum((unsigned char*)icmphdr, tmp);

    uint32_t x = iphdr->src_addr;
    iphdr->src_addr = iphdr->dst_addr;
    iphdr->dst_addr = x;

    iphdr->protocol = IPPROTO_ICMP;

    ip_tx_ready(m);

    mbuffree(m);
}

void icmp_tx(struct mbuf *m, uint32_t ip, uint8_t type, uint8_t code)
{
    struct icmp_hdr * icmphdr = (struct icmp_hdr*) mbufpush(m, sizeof(struct icmp_hdr));
    icmphdr->type = type;
    icmphdr->code = code;
    icmphdr->cksum = checksum((unsigned char*)icmphdr, m->buffer_len);

    ip_tx(m, IPPROTO_ICMP, dst_ip);
}

void icmp_rx(struct mbuf *m, uint16_t len, struct ip_hdr *iphdr)
{
    struct icmp_hdr * icmphdr = (struct icmp_hdr*) mbufpush(m, sizeof(struct icmp_hdr));
    if (!icmphdr)
        goto fail;
    
    len -= sizeof(*icmphdr);
    if (len > m->buffer_len)
        goto fail;
    
    if(checksum((unsigned char*)icmphdr, m->buffer_len))
        panic("wrong checksum");
    
    uint8_t ttl = iphdr->ttl;
    uint32_t sip = ntohl(iphdr->src_addr);
    sockrecvicmp(m, sip, ttl);
fail:
    mbuffree(m);
}