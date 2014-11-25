#ifndef __LIST_H__
#define __LIST_H__

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({  \
    const typeof( ((type*)0)->member) *__mptr = (ptr); \
    (type*)((char*)__mptr - offsetof(type, member)); })
#endif

typedef struct listnode {
    struct listnode *prev;
    struct listnode *next;
} listnode;

static inline void
list_init(struct listnode *head)
{
    head->prev = head;
    head->next = head;
}

static inline struct listnode*
list_prev(struct listnode *cur)
{
    return cur->prev;
}

static inline struct listnode*
list_next(struct listnode *cur)
{
    return cur->next;
}

static inline void
list_insert(struct listnode *prev, struct listnode *cur, struct listnode *next)
{
    prev->next = cur;
    next->prev = cur;

    cur->prev = prev;
    cur->next = next;
}

static inline void
list_remove(struct listnode *prev, struct listnode *cur, struct listnode *next)
{
    prev->next = next;
    next->prev = prev;

    cur->next = cur;
    cur->prev = cur;
}

static inline void
list_del(struct listnode *del)
{
    list_remove(del->prev, del, del->next);
}

static inline void
list_add_before(struct listnode *new, struct listnode *node)
{
    list_insert(node->prev, new, node);
}

static inline void
list_add_after(struct listnode *new, struct listnode *node)
{
    list_insert(node, new, node->next);
}

#endif
