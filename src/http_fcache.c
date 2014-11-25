#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "http_fcache.h"
#include "http_log.h"

struct http_fcache *fcache;

/*
 * BKDRHash function for filename hash.
 */
static unsigned
hash_file(const void *name)
{
    unsigned seed = 131; // 31 131 1313 13131 131313 etc
    unsigned hash = 0;
    const char *str = (const char*)name;

    while (*str) {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

static int
cmp_file(const void *lhs, const void *rhs)
{
    return strcmp((const char*)lhs, (const char*)rhs);
}

struct http_fcache*
http_fcache_create(unsigned size)
{
    struct http_fcache *cache = (struct http_fcache*)malloc(sizeof(struct http_fcache));
    if (cache != NULL) {
        hashtable_init(&cache->table, size, hash_file, cmp_file);
        list_init(&cache->lru);
    }
    return cache;
}

struct http_fcache_file*
http_fcache_putfile(struct http_fcache *cache, const char *filename)
{
    struct http_fcache_file *file = 
        (struct http_fcache_file*)malloc(sizeof(struct http_fcache_file));
    
    if (file == NULL) {
        goto malloc_fail;
    }

    if ((file->fd = open(filename, O_RDONLY, 0777)) == -1) {
        LOG_WARN("open file[%s] fail: %s", filename, strerror(errno));
        goto fail;
    }

    if (fstat(file->fd, &file->stat) == -1) {
        LOG_WARN("stat file[%s] fail: %s", filename, strerror(errno));
        goto fail;
    }

    file->name = strdup(filename);
    file->hash.key = file->name;
    hashtable_put(&cache->table, &file->hash);
    list_add_before(&file->lru, &cache->lru);

    return file;

fail:
    free(file);
malloc_fail:
    return NULL;
}

struct http_fcache_file*
http_fcache_getfile(struct http_fcache *cache, const char *filename)
{
    hashnode *hnode;

    hnode = hashtable_get(&cache->table, filename);
    if (hnode == NULL) {
        return NULL;
    }
    return container_of(hnode, struct http_fcache_file, hash);
}

void
http_fcache_activate(struct http_fcache *cache, struct http_fcache_file *file)
{
    /* not debug yet */
    list_del(&file->lru);
    list_add_after(&cache->lru, &file->lru);
}
