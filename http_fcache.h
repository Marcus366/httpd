#ifndef __HTTP_FCACHE_H__
#define __HTTP_FCACHE_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "container/hashtable.h"


struct http_fcache_file {
    int           fd;
    
    const char   *name;
    struct stat   stat;

    hashnode      hash;
    listnode      lru;
};

struct http_fcache {
    hashtable     table;
    listnode      lru;
};

struct http_fcache *fcache;

struct http_fcache* http_fcache_create(unsigned size);

struct http_fcache_file* http_fcache_getfile(struct http_fcache *cache, const char *filename);
struct http_fcache_file* http_fcache_putfile(struct http_fcache *cache, const char *filename);

/*
 * Put a node to the head of lru head.
 */
void http_fcache_activate(struct http_fcache* cache, struct http_fcache_file *file);


/*
 * TODO:
 *
 * http_fcache_passivate
 */

#endif
