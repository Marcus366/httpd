#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__


#include <unistd.h>
#include "http_mem.h"
#include "http_chain.h"
#include "http_header.h"
#include "http_mempool.h"


#define CR    '\r'
#define LF    '\n'

#define CRLF  ((u_char*)"\r\n")
#define SPACE ((u_char*)" ")

#define HTTP_BAD_REQUEST  400
#define HTTP_UNAUTHORIZED 401


/* Following are HTTP method id */
#define METHOD_UNSET   -1
#define METHOD_OPTIONS  0
#define METHOD_GET      1
#define METHOD_POST     2
#define METHOD_PUT      3
#define METHOD_DELETE   4
#define METHOD_TRACE    5
#define METHOD_CONNECT  6

/* Following are HTTP version id */
#define VERSION_UNSET -1
#define VERSION_09     0
#define VERSION_10     1
#define VERSION_11     2

/* Following are HTTP parse state */
#define PARSING_REQUEST_LINE  0
#define PARSING_REQUEST_HEAD  1
#define PARSING_REQUEST_BODY  2
#define PARSING_REQUEST_END   3
#define BUILDING_RESPONSE     4
#define SENDING_RESPONSE      5
#define CLOSING_REQUEST       6


typedef struct http_connection http_connection_t;

typedef struct http_request {
    http_connection_t *conn;

    unsigned           major_state : 4;
    unsigned           minor_state : 28;

    u_char            *read_buf;
    u_char            *read_pos;
    u_char            *read_last;
    u_char            *check_pos;

    http_chain_t      *out_chain;

    http_headers_t    *headers_in;
    http_headers_t    *headers_out;

    int                method_id;
    int                version_id;

    http_mem_t         method;
    http_mem_t         uri;
    http_mem_t         version;

    http_mempool_t    *pool;
} http_request_t;


http_request_t* new_http_request(http_connection_t *conn);
void            free_http_request(http_request_t *req);


ssize_t http_recv_request(http_request_t *req, int sockfd);
int     http_parse_request(http_request_t *req);

int     http_build_response(http_request_t *req);
int     http_send_response(http_request_t *req);


#endif

