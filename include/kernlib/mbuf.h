#ifndef MBUF
#define MBUF

#include <common/type.h>

#define MBUF_SIZE 4096
#define MBUF_DEFAULT_HEADROOM  128

// [mbuf struct][ header ][    buffer     ][ tail ]
//             - all_buf here
//                       - header_end here
//                       <-- buffer_len -->
struct mbuf {
    struct mbuf* next;
    char*        header_end;
    uint8_t      buffer_len;
    char         all_buf[MBUF_SIZE];
}__attribute__((__packed__));

// add len to the header size
// start: [header][   buffer   ][tail] 
// end  : [header   ][buffer   ][tail]
//               - return value is here
// buffer 的头部转移给 header，返回原来 buffer 的头部
char *mbufpull(struct mbuf* m, unsigned int len);

// pop len of the header size
// start: [header   ][buffer   ][tail] 
// end  : [header][   buffer   ][tail]
//               - return value is here
// header 的尾部转移给 buffer，返回现在 buffer 的头部
char *mbufpush(struct mbuf* m, unsigned int len);


// start: [header][buffer][tail] 
// end  : [header][buffer   ][tail]
//                       - return value is here
// tail 的头部部转移给 buffer，返回原来 tail 的头部
char *mbufput(struct mbuf* m, unsigned int len);

// start: [header][buffer   ][tail] 
// end  : [header][buffer][tail]
//                       - return value is here
// buffer 的尾部部转移给 tail，返回现在 tail 的头部
char *mbuftrim(struct mbuf* m, unsigned int len);


// memory operation
struct mbuf* mbufalloc(unsigned int hdr_size);
void mbuffree(struct mbuf* m);

// queue of mbuf
struct mbuf_queue {
    struct mbuf* head;
    struct mbuf* tail;
};

void mbuf_queue_pushtail(struct mbuf_queue *q, struct mbuf *m);
struct mbuf* mbuf_queue_pophead(struct mbuf_queue *q);
int mbuf_queue_empty(struct mbuf_queue *q);
void mbuf_queue_init(struct mbuf_queue *q);

#endif