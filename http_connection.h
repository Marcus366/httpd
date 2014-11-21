#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include "http_request.h"

enum conn_state {
    CONN_IDLE       = 0,
    CONN_READ       = 1,
    CONN_WRITE      = 2,
    CONN_WAIT_CLOSE = 3,
};

typedef unsigned long long ull;

typedef struct http_connection {
    struct http_srv *srv;

    ull              uuid;
    int              sockfd;
    enum conn_state  state;
    http_request_t  *req;
    struct http_res *res;
} http_connection_t;

#define SET_CONN_STATE(conn, newstate) do { conn->state = newstate; } while (0);

http_connection_t* new_http_connection(struct http_srv* srv, int sockfd);
void              free_http_connection(http_connection_t* conn);

int handle_new_connection(struct http_srv* svc);

int http_close_connection(http_connection_t* conn);

int handle_read(http_connection_t* conn);
int handle_write(http_connection_t* conn);

#endif
