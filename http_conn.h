#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

enum conn_state {
    CONN_IDLE,
    CONN_READ,
    CONN_WRITE
};

struct http_conn {
    struct http_srv *srv;

    int sockfd;
    enum conn_state state;
    struct http_req *req;
    struct http_res *res;
};

struct http_conn* new_http_conn(struct http_srv* srv, int sockfd);

int handle_new_connect(struct http_srv* svc);
int handle_read(struct http_conn* conn);
int handle_write(struct http_conn* conn);

#endif
