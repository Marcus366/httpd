#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

int main(int argc, char** argv) {
    struct sockaddr_in cli_addr;
    int conn_fd;

    conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(80);
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(conn_fd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) != 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    char buf[1024] = "GET /path HTTP/1.1\r\nHost: localhost\r\nuser-agent: Mozilla4.0\r\nConnect:Keep-Alive";
    ssize_t nread;

    write(conn_fd, buf, strlen(buf));
    while ((nread = read(conn_fd, buf, 1024)) > 0) {
        write(0, buf, nread);
    }

    exit(-1);
}
