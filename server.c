#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "server.h"
#include "connect.h"

struct server* new_http_server(int port)
{
    int listenfd, epollfd;
    struct sockaddr_in addr;

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
    
    struct server* svc = (struct server*)malloc(sizeof(struct server));
    svc->listenfd = listenfd;
    svc->epollfd = epollfd;
    return svc;
}

void serve(struct server* svc)
{
    int nfds;
    struct epoll_event ev, events[10];

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
        nfds = epoll_wait(svc->epollfd, events, 10, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == svc->listenfd) {
                handle_new_connect(svc);
            } else {
                struct connect *conn = events[i].data.ptr;
                handle_transfer(conn);
            }
        }
    }
}
