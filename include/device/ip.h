#ifndef _ORANGES_IP_H
#define _ORANGES_IP_H

#include <device/ethernet.h>

#define IP_HDR_LEN sizeof(struct ip_hdr)

// values of ip_hdr.protocol

#define IPPROTO_ICMP 1  // Control message protocol
#define IPPROTO_TCP 6   // Transmission control protocol
#define IPPROTO_UDP 17  // User datagram protocol

struct ip_hdr {
    uint8_t  vhl;
    uint8_t  tos;            // type of service
    uint16_t len;
    uint16_t id;             // identification
    uint16_t frag_offset;
    uint8_t  ttl;            // time to live
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_addr;       // source address
    uint32_t dst_addr;       // destination ip address
};

#define local_ip MAKE_IP_ADDR(192, 168, 76, 15)

void ip_rx(struct mbuf *m);
void ip_tx(struct mbuf *m, uint8_t protocol, uint32_t ip);
void ip_tx_ready(struct mbuf* m);

#endif