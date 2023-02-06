#include <kernel/memman.h>
#include <kernlib/mbuf.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

char *mbufstart = NULL;
char *mbufpointer = NULL;
char *mbufend = NULL;

#define MAX_TOTAL_MBUF (512 * KB)

// 保证所有 mbuf 获得的区块是连续的
void mbuf_init()
{
    mbufstart = NULL;
    mbufpointer = NULL;
    mbufend = NULL;
    mbufstart = (char *)do_kmalloc(MAX_TOTAL_MBUF);
    mbufpointer = mbufstart;
    mbufend = mbufstart + MAX_TOTAL_MBUF;
    return;
}

void mbuf_end()
{
    do_free((u32)mbufstart, MAX_TOTAL_MBUF);
    return;
}

/**
 * 使用空间减小
*/
char *mbufpull(struct mbuf *m, unsigned int len)
{
    char *tmp = m->head;
    // error when the len of mbuf less than the required length
    if (m->len < len)
        return 0;
    m->len -= len;
    m->head += len;
    return tmp;
}

/**
 * 使用空间增大
*/
char *mbufpush(struct mbuf *m, unsigned int len)
{
    char *tmp = m->head - len;
    m->head -= len;
    if (m->head < m->buf) {
        // TODO : panic
        return 0;
    }
    m->len += len;
    return tmp;
}

char *mbuftrim(struct mbuf *m, unsigned int len)
{

    if (len > m->len)
        return 0;
    m->len -= len;
    return m->head + m->len;
}

char *mbufput(struct mbuf *m, unsigned int len)
{
    char *tmp = m->head + m->len;
    if (m->len + len > MBUF_SIZE) {
        // TODO : panic
        return 0;
    }
    m->len += len;
    return tmp;
}

struct mbuf *mbufalloc(unsigned int hdr_size)
{
    struct mbuf *m;

    if (hdr_size > MBUF_SIZE)
        return 0;
    if (hdr_size + (u32)mbufpointer > (u32)mbufend)
        return 0;
    m = (struct mbuf *)mbufpointer;
    mbufpointer += hdr_size;

    if (m == NULL)
        return 0;

    m->next = 0;
    m->head = (char *)((u32)m->buf + (u32)hdr_size); 
    m->len = 0;
    return m;
}

void mbuffree(struct mbuf *m)
{
    mbufpointer = mbufpointer - ((uint32_t)m->head - (uint32_t)m->buf);
    return;
}

// ------------mbuf_queue-------------

void mbuf_queue_pushtail(struct mbuf_queue *q, struct mbuf *m)
{
    q->tail->next = m;
    q->tail = m;
}

struct mbuf *mbuf_queue_pophead(struct mbuf_queue *q)
{
    struct mbuf *tmp = q->head;
    kprintf("living %d \n", tmp->next);
    q->head = q->head->next;
    return tmp;
}

int mbuf_queue_empty(struct mbuf_queue *q)
{
    if (q->head == NULL)
        return 1;
    return 0;
}

void mbuf_queue_init(struct mbuf_queue *q)
{
    q->head = NULL;
}