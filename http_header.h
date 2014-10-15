#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__

#include "container/hashtable.h"

typedef struct http_headers {
    unsigned count;

    /* Use linked-hashmap to store header. */
    hashtable table;
    listnode  list;

} http_headers;

typedef struct http_header {
    char       *attr;
    char       *value;

    hashnode    hash;
    listnode    link;
} http_header;


void http_headers_init(http_headers *headers);


http_header* http_header_set(http_headers *headers, char *attr, char *value);
http_header* http_header_get(http_headers *headers, char *attr);


#endif
