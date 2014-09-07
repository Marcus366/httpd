#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

int main(int argc, char** argv)
{
    struct epoll_event ev, events[10];
    int listen_fd, conn_fd, nfds, epollfd, i;
    struct sockaddr_in addr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 1024) != 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        struct sockaddr_in conn_addr;
        socklen_t conn_len;
        bzero(&conn_addr, sizeof(conn_addr));
        nfds = epoll_wait(epollfd, events, 10, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listen_fd) {
                if ((conn_fd = accept(listen_fd, (struct sockaddr*)&conn_addr, &conn_len)) == -1) {
                    perror("accept");
                    break;
                }
                printf("accept address: %s\n", inet_ntoa(conn_addr.sin_addr));
                struct epoll_event rwev;
                rwev.events = EPOLLIN;
                rwev.data.fd = conn_fd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &rwev) == -1) {
                    perror("epoll_ctl");
                    close(conn_fd);
                    continue;
                }
            } else {
                conn_fd = events[i].data.fd;
                char buf[1024];
                ssize_t nread;
                if ((nread = read(conn_fd, buf, 1024)) > 0) {
                    write(0, buf, nread);
                }
                strcpy(buf, "HTTP/1.1 200 OK\r\nServer: Nginx\r\nDate: Sat, 31 Dec 2014 23:59:59 GMT\r\nContent-Type: text/html\r\nContent-Length: 122\r\n\r\n<html>\r\n<head>\r\n<title>Wrox Homepage</title>\r\n</head>\r\n<body>\r\n</bod>\r\n</html>\r\n");
                write(conn_fd, buf, strlen(buf));
                close(conn_fd);
            }
        }
    }

    exit(-1);
}
