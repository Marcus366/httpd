#include <string.h>
#include "http_header.h"


static unsigned
hash_mem(const void *name)
{
    unsigned seed = 131; // 31 131 1313 13131 131313 etc
    unsigned hash = 0;
    uint64_t i;

    http_mem_t *mem = (http_mem_t*)name;

    for (i = 0; i < mem->len && i < sizeof(uint64_t) * 8; ++i) {
        hash = hash * seed + mem->base[i];
    }

    return (hash & 0x7FFFFFFF);
}


static int
cmp_mem(const void *lhs, const void *rhs)
{
    http_mem_t *mem1 = (http_mem_t*)lhs;
    http_mem_t *mem2 = (http_mem_t*)rhs;

    if (mem1->base != mem2->base) {
        return mem1->base - mem2->base;
    }

    return mem1->len - mem2->len;
}


http_headers_t*
http_headers_new()
{
    http_headers_t *headers = (http_headers_t*)malloc(sizeof(http_headers_t));

    if (headers != NULL) {
      headers->count = 0;
      hashtable_init(&headers->table, 10, hash_mem, cmp_mem);
      list_init(&headers->list);
    }

    return headers;
}


void
http_headers_free(http_headers_t *headers)
{
    http_header_t *header;
    listnode *node;

    if (headers == NULL) {
        return;
    }

    node = headers->list.next;
    header = container_of(node, http_header_t, link);
    while (&header->link != &headers->list) {
        list_del(&header->link);
        free(header);

        node = headers->list.next;
        header = container_of(node, http_header_t, link);
    }

    hashtable_free(&headers->table);

    free(headers);
}


http_header_t*
http_header_set(http_headers_t *headers, http_mem_t attr, http_mem_t value)
{
    http_header_t *header;

    header = http_header_get(headers, attr);
    if (header != NULL) {
        header->value = value;
        return header;
    }

    header = (http_header_t*)malloc(sizeof(http_header_t));
    header->attr  = attr;
    header->value = value;

    header->hash.key = &attr;
    hashtable_put(&headers->table, &header->hash);

    list_add_before(&header->link, &headers->list);

    return header;
}


http_header_t*
http_header_get(http_headers_t *headers, http_mem_t attr)
{
    hashnode *node;

    node = hashtable_get(&headers->table, &attr);
    if (node == NULL) {
        return NULL;
    } else {
        return container_of(node, http_header_t, hash);
    }
}

