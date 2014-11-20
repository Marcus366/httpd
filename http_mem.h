#ifndef __HTTP_MEM_H__
#define __HTTP_MEM_H__

#include <stdint.h>
#include <sys/types.h>


#define http_mem_null() http_mem_create((u_char*)0, 0)
#define http_mem_is_null(mem) (mem.base == 0 && mem.len == 0)

#define http_mem_cmp_len(lhs, rhs) (lhs.len - rhs.len)

typedef struct http_mem {
    u_char *base;
    uint64_t len;
} http_mem_t;


http_mem_t http_mem_create(u_char *base, uint64_t len);


http_mem_t http_mem_cut(http_mem_t mem, http_mem_t delim);


int http_mem_equal(http_mem_t lhs, http_mem_t rhs);


int http_mem_cmp(http_mem_t lhs, http_mem_t rhs);


void http_mem_print(http_mem_t mem);


#endif

