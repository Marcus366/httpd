#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "server.h"
#include "connect.h"

struct connect* new_connect(int sockfd)
{
    struct connect* ptr = (struct connect*)malloc(sizeof(struct connect));
    if (ptr != NULL) {
        ptr->sockfd = sockfd;
        ptr->state = CONN_IDLE;
        ptr->req = NULL;
    }
    return ptr;
}

int handle_new_connect(struct server* svc)
{
    int connfd;
    socklen_t conn_len = 0;
    struct sockaddr_in conn_addr;
    memset(&conn_addr, 0, sizeof(conn_addr));

    if ((connfd = accept(svc->listenfd, (struct sockaddr*)&conn_addr, &conn_len)) == -1) {
        perror("accept");
        return -1;
    }
    int fl = fcntl(connfd, F_GETFL);
    if (fl == -1) {
        perror("getfl");
        close(connfd);
        return -1;
    }
    if (fcntl(connfd, F_SETFL, fl & O_NONBLOCK) == -1) {
        perror("setfl");
        close(connfd);
        return -1;
    }

    printf("accept address: %s\n", inet_ntoa(conn_addr.sin_addr));
    
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = new_connect(connfd);
    if (epoll_ctl(svc->epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
        perror("epoll_ctl");
        free(ev.data.ptr);
        close(connfd);
        return -1;
    }
    return 0;
}

int handle_transfer(struct connect* conn)
{
    char buf[1024];
    ssize_t nread;
    if ((nread = read(conn->sockfd, buf, 1024)) > 0) {
        write(0, buf, nread);
    }
    strcpy(buf, "HTTP/1.1 200 OK\r\nServer: Nginx\r\nDate: Sat, 31 Dec 2014 23:59:59 GMT\r\nContent-Type: text/html\r\nContent-Length: 122\r\n\r\n<html>\r\n<head>\r\n<title>Wrox Homepage</title>\r\n</head>\r\n<body>\r\n</bod>\r\n</html>\r\n");
    write(conn->sockfd, buf, strlen(buf));
    close(conn->sockfd);

    return 0;
}
