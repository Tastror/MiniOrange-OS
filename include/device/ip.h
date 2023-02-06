#ifndef _ORANGES_IP_H
#define _ORANGES_IP_H

#include <device/ethernet.h>

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

#define dst_ip MAKE_IP_ADDR(192, 168, 76, 2)

#endif