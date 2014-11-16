#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <unistd.h>
#include "http_header.h"

#define CR '\r'
#define LF '\n'

#define METHOD_GET  1
#define METHOD_POST 2

#define PARSING_REQUEST_LINE 0
#define PARSING_REQUEST_HEAD 1
#define PARSING_REQUEST_BODY 2
#define PARSING_REQUEST_END  3


typedef enum request_state {
    REQ_INVAL,

    REQ_PARSE_METHOD_BEGIN,
    REQ_PARSING_METHOD,

    REQ_PARSE_URI_BEGIN,
    REQ_PARSING_URI,

    REQ_PARSE_VERSION_BEGIN,
    REQ_PARSING_VERSION,

    REQ_PARSE_REQLINE_END,

    REQ_CR,
    REQ_CRLF,
    REQ_CRLFCR,
    REQ_PARSE_END
} request_state_t;

typedef struct http_request {
    unsigned         major_state : 4;
    unsigned         minor_state : 28;

    u_char          *read_buf;
    size_t           buf_size;

    size_t           read_idx;
    size_t           check_idx;

    const char      *method;
    const char      *uri;
    const char      *version;
} http_request_t;

http_request_t* new_http_request(size_t bufsize);
void            free_http_request(http_request_t *req);

ssize_t http_recv_request(http_request_t *req, int sockfd);
int     http_parse_request(http_request_t *req);

#endif
