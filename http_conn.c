#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "http_srv.h"
#include "http_conn.h"
#include "http_request.h"
#include "http_res.h"
#include "http_timer.h"
#include "http_log.h"

ull conn_count = 0;

struct http_conn* new_http_conn(struct http_srv* srv, int sockfd)
{
    struct http_conn* conn = (struct http_conn*)malloc(sizeof(struct http_conn));
    if (conn != NULL) {
        conn->uuid   = conn_count++;
        conn->srv    = srv;
        conn->sockfd = sockfd;
        conn->state  = CONN_READ;
        conn->req    = NULL;
        conn->res    = NULL;
    }
    return conn;
}

void free_http_conn(struct http_conn* conn)
{
    if (conn->req) {
        free_http_request(conn->req);
    }
    if (conn->res) {
        free_http_res(conn->res);
    }
    free(conn);
}

int handle_new_connect(struct http_srv* srv)
{
    int connfd;
    socklen_t conn_len = 0;
    struct sockaddr_in conn_addr;

    for (;;) {
        memset(&conn_addr, 0, sizeof(conn_addr));
        conn_len = sizeof(conn_addr);

        if ((connfd = accept(srv->listenfd, (struct sockaddr*)&conn_addr, &conn_len)) == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                perror("accept");
                return -1;
            }
        }
        int fl = fcntl(connfd, F_GETFL);
        if (fl == -1) {
            perror("getfl");
            close(connfd);
            return -1;
        }
        if (fcntl(connfd, F_SETFL, fl | O_NONBLOCK) == -1) {
            perror("setfl");
            close(connfd);
            return -1;
        }

        struct http_conn *conn = new_http_conn(srv, connfd);
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = conn;
        if (epoll_ctl(srv->epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
            perror("epoll_ctl");
            http_close_conn(ev.data.ptr);
            return -1;
        }
        LOG_INFO("accept address %s, sockfd:%d, uuid:%llu", inet_ntoa(conn_addr.sin_addr), conn->sockfd, conn->uuid);
    }
#ifdef __DEBUG__
    if (conn_count > 10000) {
        exit(EXIT_SUCCESS);
    }
#endif
    return 0;
}

void http_close_cb(void* arg)
{
    struct http_conn *conn = (struct http_conn*)arg;
    LOG_DEBUG("http_close_cb");
    shutdown(conn->sockfd, SHUT_RD);
    free_http_conn(conn);
}

int http_close_conn(struct http_conn* conn)
{
    int ret = close(conn->sockfd);
    free_http_conn(conn);
    return ret;
}

int handle_read(struct http_conn* conn)
{
    if (conn->req == NULL) {
        conn->req = new_http_request(1024);
    }

    http_recv_request(conn->req, conn->sockfd);

    if (http_parse_request(conn->req)) {
        LOG_DEBUG("parse request: method:%s uri:%s version:%s",
                conn->req->method.base, conn->req->uri.base, conn->req->version.base);
        struct epoll_event ev;
        ev.events = EPOLLOUT | EPOLLET;
        ev.data.ptr = conn;
        if (epoll_ctl(conn->srv->epollfd, EPOLL_CTL_MOD, conn->sockfd, &ev) == -1) {
            perror("epoll mod");
            http_close_conn(conn);
            return -1;
        }
        conn->state = CONN_WRITE; 
    }

    return 0;
}

int handle_write(struct http_conn* conn)
{
    if (conn->res == NULL) {
        conn->res = new_http_res();
        if (http_gen_res(conn->res, conn->req) == -1) {
            http_close_conn(conn);
            return -1;
        }
    }

    if (http_send_res(conn->res, conn->sockfd) == SEND_FINISH) {
        LOG_VERBOSE("SEND_FINISH");
        SET_CONN_STATE(conn, CONN_WAIT_CLOSE);
        http_close_conn(conn);
        //shutdown(conn->sockfd, SHUT_WR);
        //http_timer_create(1e6, http_close_cb, conn, TIMER_ONCE);
    }
    return 0;
}
