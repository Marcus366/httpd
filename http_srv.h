#ifndef __SERVER_H__
#define __SERVER_H__

struct http_srv {
    int listenfd;
    int epollfd;
};

struct http_srv* new_http_srv(int port);

void serve(struct http_srv* svc);

#endif
