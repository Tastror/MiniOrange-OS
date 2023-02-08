#ifndef _ORANGES_SOCKET_H
#define _ORANGES_SOCKET_H

#include <kernel/spinlock.h>
#include <kernlib/mbuf.h>

struct socket {
    struct socket *next; // the next socket in the list
    uint8_t protocol_type; // the procotol type in ip
    uint32_t r_ip; // the remote IPv4 address
    uint16_t lport; // the local UDP port number
    uint16_t rport; // the remote UDP port number
    uint8_t icmp_type;
    uint8_t icmp_code;
    uint8_t icmp_recvttl;
    struct spinlock lock;
    struct mbuf_queue rxq;
};

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE, FD_SOCK } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  size_t off;          // FD_INODE
  short major;       // FD_DEVICE
  struct socket* sock;
};

void sockinit(void);
int udp_sockalloc(struct file **f, uint32_t r_ip, uint16_t lport, uint16_t rport);
int icmp_sockalloc(struct file ** f, uint32_t r_ip, uint8_t icmp_type, uint8_t icmp_code);
void sockclose(struct socket * si);
int sockread(struct socket *si, uint64_t addr, int n);
int sockwrite(struct socket *si, uint64_t addr, int n);
void sockrecvudp(struct mbuf *m, uint32_t r_ip, uint16_t lport, uint16_t rport);
void sockrecvicmp(struct mbuf *m, uint32_t r_ip, uint8_t ttl);

#endif