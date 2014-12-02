#include <stdlib.h>
#include "http_chain.h"


http_chain_t*
http_mem_chain_create(http_mempool_t *pool, http_mem_t mem)
{
    http_chain_t *chain;

    chain = (http_chain_t*)http_mempool_alloc(pool, sizeof(http_chain_t));
    if (chain == NULL) {
        return NULL;
    }

    chain->type = MEMORY_CHAIN;
    chain->data.mem = mem;
    chain->offset.mem_off = 0;
    chain->next = NULL;

    return chain;
}


http_chain_t*
http_sendfile_chain_create(http_mempool_t *pool, int fd, off_t size)
{
    http_chain_t *chain;

    chain = (http_chain_t*)http_mempool_alloc(pool, sizeof(http_chain_t));
    if (chain == NULL) {
        return NULL;
    }

    chain->type = SENDFILE_CHAIN;
    chain->data.sendfile.fd = fd;
    chain->data.sendfile.size = size;
    chain->offset.file_off = 0;
    chain->next = NULL;

    return chain;
}

