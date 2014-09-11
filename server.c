#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "server.h"

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
        struct sockaddr_in conn_addr;
        int connfd;
        socklen_t conn_len;
        memset(&conn_addr, 0, sizeof(conn_addr));
        nfds = epoll_wait(svc->epollfd, events, 10, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == svc->listenfd) {
                if ((connfd = accept(svc->listenfd, (struct sockaddr*)&conn_addr, &conn_len)) == -1) {
                    perror("accept");
                    break;
                }
                printf("accept address: %s\n", inet_ntoa(conn_addr.sin_addr));
                struct epoll_event rwev;
                rwev.events = EPOLLIN | EPOLLET;
                rwev.data.fd = connfd;
                if (epoll_ctl(svc->epollfd, EPOLL_CTL_ADD, connfd, &rwev) == -1) {
                    perror("epoll_ctl");
                    close(connfd);
                    continue;
                }
            } else {
                connfd = events[i].data.fd;
                char buf[1024];
                ssize_t nread;
                if ((nread = read(connfd, buf, 1024)) > 0) {
                    write(0, buf, nread);
                }
                strcpy(buf, "HTTP/1.1 200 OK\r\nServer: Nginx\r\nDate: Sat, 31 Dec 2014 23:59:59 GMT\r\nContent-Type: text/html\r\nContent-Length: 122\r\n\r\n<html>\r\n<head>\r\n<title>Wrox Homepage</title>\r\n</head>\r\n<body>\r\n</bod>\r\n</html>\r\n");
                write(connfd, buf, strlen(buf));
                close(connfd);
            }
        }
    }
}
