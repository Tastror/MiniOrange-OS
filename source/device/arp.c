#include <device/arp.h>
#include <device/ip.h>
#include <kernlib/mbuf.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

uint8_t local_mac[ETHADDR_LEN] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};

void arp_tx(uint16_t op, uint8_t desmac[ETHADDR_LEN], uint32_t tip)
{
    struct arp_hdr *hdr;
    struct mbuf    *m;

    m = mbufalloc(sizeof(struct eth_hdr) + sizeof(struct arp_hdr));

    hdr = (struct arp_hdr *)mbufpush(m, sizeof(struct arp_hdr));

    hdr->ar_hrd = htons(ARP_HRD_ETHER);
    hdr->ar_pro = htons(ETHTYPE_IP);
    hdr->ar_hln = ETHADDR_LEN;
    hdr->ar_pln = sizeof(uint32_t);
    hdr->ar_op = htons(op);

    memmove(hdr->arp_sha, local_mac, ETHADDR_LEN);
    hdr->arp_sip = htonl(local_ip);

    memmove(hdr->arp_tha, desmac, ETHADDR_LEN);
    hdr->arp_tip = htonl(tip);
    
    eth_tx(m, ETHTYPE_ARP);
}

uint8_t SPLIT_IP_1(uint32_t ip) {
    return (ip & 0xff000000UL) >> 24;
}

uint8_t SPLIT_IP_2(uint32_t ip) {
    return (ip & 0x00ff0000UL) >> 16;
}

uint8_t SPLIT_IP_3(uint32_t ip) {
    return (ip & 0x0000ff00UL) >> 8;
}

uint8_t SPLIT_IP_4(uint32_t ip) {
    return (ip & 0x000000ffUL);
}

void disp_ip(uint32_t ip) {
    kprintf("%d.%d.%d.%d", SPLIT_IP_1(ip), SPLIT_IP_2(ip), SPLIT_IP_3(ip), SPLIT_IP_4(ip));
}

void disp_mac(uint8_t mac[6]) {
    kprintf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void arp_rx(struct mbuf *mbuffer)
{

    kprintf("arp packet recv \n");

    struct arp_hdr *hdr;
    uint8_t         smac[ETHADDR_LEN];

    hdr = (struct arp_hdr *)mbufpull(mbuffer, sizeof(struct arp_hdr));

    if (!hdr) {
        kprintf("arp fail: null header \n");
        goto fin;
    }

    if (ntohs(hdr->ar_hrd) != ARP_HRD_ETHER ||
        ntohs(hdr->ar_pro) != ETHTYPE_IP ||
        hdr->ar_hln != ETHADDR_LEN ||
        hdr->ar_pln != sizeof(uint32_t)) {
        kprintf("arp fail\n");
        goto fin;
    }

    // if (ntohl(hdr->arp_tip) != local_ip || ntohs(hdr->ar_op) != ARP_OP_REQUEST) {
    //     kprintf("arp fail : wrong tip\n");
    //     goto fin;
    // }

    if (ntohs(hdr->ar_op) == ARP_OP_REPLY) {
        uint32_t res_ip = ntohl(hdr->arp_sip);
        uint8_t res_mac[ETHADDR_LEN];
        memmove(res_mac, hdr->arp_sha, ETHADDR_LEN);
        kern_set_color(MAGENTA);
        kprintf("arp result: ");
        disp_mac(res_mac);
        kprintf(" has ");
        disp_ip(res_ip);
        kprintf("\n");
        kern_set_color(WHITE);
        goto fin;
    }

    memmove(smac, hdr->arp_sha, ETHADDR_LEN);

    kprintf("ready to send arp\n");
    arp_tx(ARP_OP_REPLY, smac, ntohl(hdr->arp_sip));
fin:
    mbuffree(mbuffer);
}