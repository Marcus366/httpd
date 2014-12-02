#ifndef __HTTP_CHAIN_H__
#define __HTTP_CHAIN_H__

#include "http_mem.h"
#include "http_mempool.h"

#define SENDFILE_CHAIN   0
#define MEMORY_CHAIN     1

typedef struct http_chain http_chain_t;
struct http_chain {
    int type;

    union {
        http_mem_t mem;
        struct {
          int fd;
          off_t size;
        } sendfile;
    } data;

    union {
        size_t mem_off;
        off_t  file_off;
    } offset;

    http_chain_t *next;
};


http_chain_t* http_mem_chain_create(http_mempool_t *pool, http_mem_t mem);


http_chain_t* http_sendfile_chain_create(http_mempool_t *pool, int fd, off_t size);


#endif

