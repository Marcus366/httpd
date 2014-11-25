#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include "list.h"


typedef unsigned (*hash_func)(const void*);

typedef int (*hash_cmp)(const void*, const void*);

static unsigned
default_hash_func(const void *arg)
{
    const unsigned hash = (const unsigned long)arg;
    return hash * 0x9e370001;
}

static int
default_hash_cmp(const void *lhs, const void *rhs)
{
    const unsigned long l = (const unsigned long)lhs;
    const unsigned long r = (const unsigned long)rhs;
    return l - r;
}

typedef struct hashnode {
    const void   *key;
    struct listnode  list;
} hashnode;

static inline void
hnode_init(struct hashnode *node, void *key)
{
    node->key = key;
    list_init(&node->list);
}

typedef struct hashtable {
    unsigned       capacity;
    hash_func      hash;
    hash_cmp       cmp;
    struct hashnode  *elem;
} hashtable;

static inline void
hashtable_init(struct hashtable *table, unsigned initCap, hash_func hash, hash_cmp cmp)
{
    assert(initCap > 0);

    unsigned i, capacity;

    if (hash == NULL) {
        table->hash = default_hash_func;
    } else {
        table->hash = hash;
    }

    if (cmp == NULL) {
        table->cmp = default_hash_cmp;
    } else {
        table->cmp = cmp;
    }

    if (initCap > (1u << 31)) {
        capacity = (1u << 31);
    } else {
        capacity = 1;
        while (capacity < initCap) {
            capacity <<= 1;
        }
    }

    table->capacity = capacity;
    table->elem = (struct hashnode*)malloc(capacity * sizeof(struct hashnode));
    for (i = 0; i < capacity; ++i) {
        table->elem[i].key = NULL;
        list_init(&table->elem[i].list);
    }
}

static inline void
hashtable_free(struct hashtable *table)
{
    free(table->elem);
}

static inline void
hashtable_put(struct hashtable *table, struct hashnode *hnode)
{
    assert(table != NULL);
    assert(hnode != NULL);

    unsigned index;

    index = table->hash(hnode->key);
    index = index & (table->capacity - 1);
    list_add_after(&hnode->list, &table->elem[index].list);
}

static inline void
hashtable_del(struct hashtable *table, struct hashnode *hnode)
{
    assert(table != NULL);
    assert(hnode != NULL);

    list_del(&hnode->list);
}

static inline struct hashnode*
hashtable_get(struct hashtable *table, const void *key)
{
    assert(table != NULL);
    assert(key   != NULL);

    unsigned index;
    struct listnode *lnode;
    struct hashnode *hnode;

    index = table->hash(key);
    index = index & (table->capacity - 1);

    lnode = list_next(&table->elem[index].list);
    while (lnode != &table->elem[index].list) {
        hnode = container_of(lnode, struct hashnode, list);
        if (table->cmp(key, hnode->key) == 0) {
            return hnode;
        }
        lnode = list_next(lnode);
    }

    return NULL;
}

#endif
