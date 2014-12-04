#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <sys/time.h>
#include "http_listen_socket.h"


typedef struct http_server {
    const char *docroot;
    struct timeval now;

    http_listen_socket_t *listening;
} http_server_t;



#endif

