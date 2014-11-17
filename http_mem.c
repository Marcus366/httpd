#include "http_mem.h"


http_mem_t
http_mem_create(u_char *base, uint64_t len)
{
    http_mem_t ret = { .base = base, .len = len };
    return ret;
}


http_mem_t
http_mem_cut(http_mem_t mem, http_mem_t delmi)
{
    u_char *p = mem.base;
    unsigned i, offset;

    for (offset = 0; offset + delmi.len < mem.len; ++offset) {
        for (i = 0; i < delmi.len; ++i) {
            if (p[offset + i] != delmi.base[i]) {
                break;
            }
        }

        if (i == delmi.len) {
            return http_mem_create(mem.base, offset + delmi.len);
        }
    }

    return http_mem_null();
}


int
http_mem_equal(http_mem_t lhs, http_mem_t rhs)
{
    if (lhs.base == rhs.base && lhs.len == rhs.len) {
        return 1;
    }
    return 0;
}


int
http_mem_cmp(http_mem_t lhs, http_mem_t rhs)
{
    uint64_t i, len1, len2;

    len1 = lhs.len;
    len2 = rhs.len;

    for (i = 0; i < len1 && i < len2; ++i) {
        if (lhs.base[i] < rhs.base[i]) {
            return -1;
        } else if (lhs.base[i] > rhs.base[i]) {
            return 1;
        }
    }

    if (len1 < len2) {
        return -1;
    } else if (len1 > len2) {
        return 1;
    }

    return 0;
}
