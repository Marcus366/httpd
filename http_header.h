#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__

#include "container/hashtable.h"

typedef struct http_headers {
    unsigned count;

    /* Use linked-hashmap to store header. */
    hashtable table;
    listnode  list;

} http_headers_t;

typedef struct http_header {
    char       *attr;
    char       *value;

    hashnode    hash;
    listnode    link;
} http_header_t;


/* Create a http headers stroage */
http_headers_t* http_headers_new();

/* Free memory of http headers */
void http_headers_free(http_headers_t* headers);

/* Setter/Getter of http_headers */
http_header_t* http_header_set(http_headers_t *headers, char *attr, char *value);
http_header_t* http_header_get(http_headers_t *headers, char *attr);


#endif
