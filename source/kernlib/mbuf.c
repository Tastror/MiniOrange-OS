#include <kernel/memman.h>
#include <kernlib/assert.h>
#include <kernlib/mbuf.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

char *mbufstart = NULL;
char *mbufpointer = NULL;
char *mbufend = NULL;

#define MAX_TOTAL_MBUF (128 * MBUF_SIZE)  // 512 KB

// 预先分配，防止问题出在 do_kmalloc 上面
void mbuf_init()
{
    mbufstart = NULL;
    mbufpointer = NULL;
    mbufend = NULL;
    mbufstart = (char *)K_PHY2LIN(do_kmalloc(MAX_TOTAL_MBUF));
    mbufpointer = mbufstart;
    mbufend = mbufstart + MAX_TOTAL_MBUF;
    return;
}

void mbuf_end()
{
    do_free((u32)mbufstart, MAX_TOTAL_MBUF);
    return;
}

char *mbufpull(struct mbuf *m, unsigned int len)
{
    char *tmp = m->header_end;
    if (m->buffer_len < len)
        panic("mbufpull overflow");
    m->buffer_len -= len;
    m->header_end += len;
    return tmp;
}

char *mbufpush(struct mbuf *m, unsigned int len)
{
    if (m->header_end - len < m->all_buf)
        panic("mbufpush overflow");
    m->header_end -= len;
    m->buffer_len += len;
    return m->header_end;
}

char *mbuftrim(struct mbuf *m, unsigned int len)
{
    if (len > m->buffer_len)
        panic("mbuftrim overflow");
    m->buffer_len -= len;
    return m->header_end + m->buffer_len;
}

char *mbufput(struct mbuf *m, unsigned int len)
{
    char *tmp = m->header_end + m->buffer_len;
    if (m->buffer_len + len > MBUF_SIZE)
        panic("mbufput overflow");
    m->buffer_len += len;
    return tmp;
}

struct mbuf *mbufalloc(unsigned int hdr_size)
{
    struct mbuf *m;

    if (MBUF_SIZE + (u32)mbufpointer > (u32)mbufend)
        panic("mbufalloc overflow");
    mbufpointer += MBUF_SIZE;

    if (hdr_size > MBUF_SIZE)
        return NULL;

    m = (struct mbuf *)mbufpointer;

    if (m == NULL)
        return 0;

    memset(m, 0, MBUF_SIZE);
    m->next = 0;
    m->header_end = (char *)((u32)m->all_buf + (u32)hdr_size);
    m->buffer_len = 0;
    return m;
}

void mbuffree(struct mbuf *m)
{
    if (mbufpointer <= mbufstart)
        panic("mbuffree overflow");
    mbufpointer -= MBUF_SIZE;
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