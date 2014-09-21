#ifndef __SERVER_H__
#define __SERVER_H__

struct http_srv {
    int listenfd;
    int epollfd;
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
