#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "http_srv.h"
#include "http_conn.h"
#include "http_timer.h"

struct http_srv* new_http_srv(int port)
{
    int listenfd, epollfd;
    struct sockaddr_in addr;

    http_timer_init();

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        return NULL;
    }

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("bind");
        return NULL;
    }

    epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    
    struct http_srv* svc = (struct http_srv*)malloc(sizeof(struct http_srv));
    svc->listenfd = listenfd;
    svc->epollfd = epollfd;
    return svc;
}

void serve(struct http_srv* svc)
{
    int nfds;
    struct epoll_event ev, events[1024];

    if (listen(svc->listenfd, 1024) != 0) {
        perror("listen");
        return;
    }
    
    ev.events = EPOLLIN;
    ev.data.fd = svc->listenfd;
    if (epoll_ctl(svc->epollfd, EPOLL_CTL_ADD, svc->listenfd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        return;
    }

    for (;;) {
        http_timer_run();

        nfds = epoll_wait(svc->epollfd, events, 1024, 500);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == svc->listenfd) {
                handle_new_connect(svc);
            } else {
                struct http_conn *conn = events[i].data.ptr;
                if (conn->state == CONN_READ) {
                    handle_read(conn);
                } else if (conn->state == CONN_WRITE) {
                    handle_write(conn);
                } else {
                    printf("%s line%d invalid conn state\n", __FILE__, __LINE__);
                }
            } 
        }
    }
}
