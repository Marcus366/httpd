#ifndef __HTTP_FCACHE_H__
#define __HTTP_FCACHE_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "container/htable.h"


struct http_fcache_file {
//    void         *addr;
    int           fd;
    
    const char   *name;
    struct stat   stat;

    struct hnode  hash;
    struct lnode  lru;
};

struct http_fcache {
    struct htable table;
    struct lnode  lru;
};

struct http_fcache *fcache;

struct http_fcache* http_fcache_create(unsigned size);

struct http_fcache_file* http_fcache_getfile(struct http_fcache *cache, const char *filename);
struct http_fcache_file* http_fcache_putfile(struct http_fcache *cache, const char *filename);

#endif
