#include <string.h>

#include "http_header.h"

static unsigned
hash_str(const void *name)
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
cmp_str(const void *lhs, const void *rhs)
{
    return strcmp((const char*)lhs, (const char*)rhs);
}

void
http_headers_init(http_headers *headers)
{
    headers->count = 0;
    hashtable_init(&headers->table, 10, hash_str, cmp_str);
    list_init(&headers->list);
}

http_header*
http_header_set(http_headers *headers, char *attr, char *value)
{
    http_header *header;

    header = http_header_get(headers, attr);
    if (header != NULL) {
        header->value = value;
        return header;
    }

    header->attr  = attr;
    header->value = value;

    hashtable_put(&headers->table, &header->hash);
    list_add_before(&header->link, &headers->list);
}

http_header*
http_header_get(http_headers *headers, char *attr)
{
    hashnode *node;

    node = hashtable_get(&headers->table, attr);
    if (node == NULL) {
        return NULL;
    } else {
        return container_of(node, http_header, hash);
    }
}
