#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <time.h>

struct http_srv {
    int listenfd;
    int epollfd;

    struct timeval *now;
};

/*
 * New a http server.
 * Listen the given port.
 */
struct http_srv* new_http_srv(int port);

/*
 * This function are assumed never returned.
 */
void serve(struct http_srv* svc);

#endif
