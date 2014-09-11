#ifndef __SERVER_H__
#define __SERVER_H__

struct server {
    int listenfd;
    int epollfd;
};

struct server* new_http_server(int port);

void serve(struct server* svc);

#endif
