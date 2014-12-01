#ifndef __HTTP_MEMPOOL_H__
#define __HTTP_MEMPOOL_H__


#include <sys/types.h>

typedef struct http_mempool http_mempool_t;
struct http_mempool {
    http_mempool_t *next;

    u_char *pos;
    u_char *last;
};


http_mempool_t* http_mempool_create(size_t size);


void http_mempool_free(http_mempool_t *pool);


void* http_mempool_alloc(http_mempool_t *pool, size_t size);
void* http_mempool_alloc(http_mempool_t *pool, void *ptr, size_t size);


#endif

