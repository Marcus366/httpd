#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <sys/time.h>


typedef struct http_server {
    int listenfd;
    int epollfd;

    struct timeval now;
} http_server_t;


/*
 * Create a http server and listen the given port.
 */
http_server_t* new_http_server(int listenfd);

/*
 * This function are assumed never returned.
 */
void serve(http_server_t* svc);

#endif

