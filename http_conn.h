#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

enum conn_state {
    CONN_IDLE       = 0,
    CONN_READ       = 1,
    CONN_WRITE      = 2,
    CONN_WAIT_CLOSE = 3,
};

struct http_conn {
    struct http_srv *srv;

    int              sockfd;
    enum conn_state  state;
    struct http_req *req;
    struct http_res *res;
};

#define SET_CONN_STATE(conn, newstate) do { conn->state = newstate; } while (0);

struct http_conn* new_http_conn(struct http_srv* srv, int sockfd);
void              free_http_conn(struct http_conn* conn);

int handle_new_connect(struct http_srv* svc);

int http_close_conn(struct http_conn* conn);

int handle_read(struct http_conn* conn);
int handle_write(struct http_conn* conn);

#endif
