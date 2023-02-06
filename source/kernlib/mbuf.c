#include <kernel/memman.h>
#include <kernlib/assert.h>
#include <kernlib/mbuf.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

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
    if (hdr_size > MBUF_SIZE)
        return NULL;

    u32 alloc = do_kmalloc(MBUF_SIZE);
    if (alloc == -1)
        panic("mbufalloc overflow");
    alloc = K_PHY2LIN(alloc);

    struct mbuf *m = (struct mbuf *)alloc;

    memset(m, 0, MBUF_SIZE);
    m->next = 0;
    m->header_end = (char *)((u32)m->all_buf + (u32)hdr_size);
    m->buffer_len = 0;
    return m;
}

void mbuffree(struct mbuf *m)
{
    u32 res = do_free((u32)m, MBUF_SIZE);
    if (res == -1)
        panic("mbuffree overflow");
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