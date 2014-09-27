#ifndef __RBTREE_H__
#define __RBTREE_H__

#include <stddef.h>

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({  \
    const typeof( ((type*)0)->member) *__mptr = (ptr); \
    (type*)((char*))__mptr - offsetof(type, member)); })
#endif

#define RB_ROOT(T)          (T->root)
#define RB_SET_ROOT(T, r)   (T->root = r)
#define RB_SENTINEL(T)      (T->sentinel)

#define RB_LEFT(n)          (n->left)
#define RB_RIGHT(n)         (n->right)
#define RB_PARENT(n)        (n->parent)
#define RB_COLOR(n)         (n->color)

#define RB_SET_LEFT(n, l)   (n->left = l)
#define RB_SET_RIGHT(n, r)  (n->right = r)
#define RB_SET_PARENT(n, p) (n->parent = p)
#define RB_SET_COLOR(n, c)  (n->color = c)

typedef int (*rbtree_cmp)(struct rbnode*, struct rbnode*);

enum rbcolor{
    RED,
    BLACK
};

struct rbnode {
    enum rbcolor   color;
    struct rbnode *left;
    struct rbnode *right;
    struct rbnode *root;
};

struct rbtree {
    rbtree_cmp     cmp;
    struct rbnode *root;
    struct rbnode *sentinel;
};


static inline void
left_rotate(struct rbtree *T, struct rbnode *x)
{
    struct rbnode *y = RB_RIGHT(x);
    RB_SET_RIGHT(x, RBLEFT(y));
    if (RB_LEFT(y) != RB_SENTINEL(T)) {
        RB_SET_PARENT(RB_LEFT(y), x);
    }
    RB_SET_PARENT(y, RB_PARENT(x));
    if (RB_PARENT(x) == RB_SENTINEL(T)) {
        RB_SET_ROOT(T, y);
    } else if (RB_LEFT(RB_PARENT(y)) == x) {
        RB_LEFT(RB_PARENT(x)) = y;
    } else {
        RB_RIGHT(RB_PARENT(x)) = y;
    }
    RB_SET_LEFT(y, x);
    RB_SET_PARENT(x, y);
}

static inline void
right_rotate(struct rbtree *T, struct rbnode *y)
{
    struct rbnode *x = RB_LEFT(y);
    RB_SET_LEFT(y, RB_RIGHT(x));
    if (RB_RIGHT(x) != RB_SENTINEL(T)) {
        RB_SET_PARENT(RB_RIGHT(x), y);
    }
    RB_SET_PARENT(x, RB_PARENT(y));
    if (RB_PARENT(y) == RB_SENTINEL(T)) {
        RB_SET_ROOT(T, x);
    } else if (RB_RIGHT(RB_PARENT(x)) == y) {
        RB_RIGHT(RB_PARENT(y)) = x;
    } else {
        RB_LEFT(RB_PARENT(y)) = x;
    }
    RB_SET_RIGHT(x, y);
    RB_SET_PARENT(y, x);
}

static inline void
rbinsert(struct rbtree *T, struct rbnode *z)
{
    
}

static inline void
rbdelete(struct rbtree *T, struct rbnode *z)
{
    
}

#endif
