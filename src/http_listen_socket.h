#ifndef __HTTP_LISTEN_SOCKET_H__
#define __HTTP_LISTEN_SOCKET_H__


#include "httpd.h"
#include "http_server.h"


typedef struct http_listen_socket http_listen_socket_t;
struct http_listen_socket {
    int                   fd;
    int                   port;

    http_server_t        *servers;

    http_listen_socket_t *next;
};


#endif

