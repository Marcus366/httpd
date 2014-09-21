#ifndef __HTTP_REQ_H__
#define __HTTP_REQ_H__

#include <unistd.h>

#define CR '\r'
#define LF '\n'

#define METHOD_GET  1
#define METHOD_POST 2

enum req_state {
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
};

struct http_req {
    enum req_state   state;
    char            *read_buf;
    size_t           buf_size;
    size_t           read_idx;
    size_t           check_idx;

    char            *method;
    char            *uri;
    char            *version;
};

struct http_req* new_http_req(size_t bufsize);
void             free_http_req(struct http_req *req);

ssize_t http_recv_req(struct http_req *req, int sockfd);
int     http_parse_req(struct http_req *req);

#endif
