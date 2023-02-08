#ifndef _ORANGES_ICMP_H
#define _ORANGES_ICMP_H

#include <device/ip.h>
#include <kernlib/mbuf.h>

#define ICMP_REPLY            0x00
#define ICMP_DST_UNREACHABLE  0x03
#define ICMP_ECHO             0x08

struct icmp_hdr {
    uint8_t  type;
    uint8_t  code;
    uint16_t cksum;
} __attribute__((packed));

void icmp_incoming(struct mbuf* m);
void icmp_reply(struct mbuf* m);
void icmp_tx(struct mbuf* m, uint32_t ip, uint8_t type, uint8_t code);

#endif