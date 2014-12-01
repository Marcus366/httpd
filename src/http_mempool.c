#include <stdlib.h>
#include "http_mempool.h"


#define POOL_SIZE (1024 * 4)


http_mempool_t*
http_mempool_create(size_t size)
{
    http_mempool_t *pool = (http_mempool_t*)malloc(size);

    if (pool == NULL) {
        return NULL;
    }

    pool->next = NULL;

    pool->pos = (u_char*)pool + sizeof(http_mempool_t);
    pool->last = (u_char*)pool + size;

    return pool;
}


void
http_mempool_free(http_mempool_t *pool)
{
    http_mempool_t *del;

    while (pool) {
        del = pool;
        pool = pool->next;

        free(del);
    }
}


void*
http_mempool_alloc(http_mempool_t *pool, size_t size)
{
    void *ret;
    size_t free;
    http_mempool_t *p;

    for (p = pool; p; p = p->next) {
        free = p->last - p->pos;

        if (free >= size) {
            goto found;
        }
    }

    p = http_mempool_create(size > POOL_SIZE ? size : POOL_SIZE);
    if (p == NULL) {
        return NULL;
    }
    p->next = pool;

found:
    ret = p->pos;
    p->pos += size;
    return ret;
}

