#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "http_server.h"
#include "http_connection.h"
#include "http_request.h"
#include "http_timer.h"
#include "http_log.h"

ull conn_count = 0;


http_connection_t*
new_http_connection(int sockfd, http_listen_socket_t *listening)
{
    http_connection_t* conn =
        (http_connection_t*)malloc(sizeof(http_connection_t));

    if (conn == NULL) {
        return NULL;
    }

    conn->uuid      = conn_count++;
    conn->sockfd    = sockfd;
    conn->state     = CONN_READ;
    conn->listening = listening;
    conn->req       = NULL;

    return conn;
}


void
free_http_connection(http_connection_t* conn)
{
    if (conn->req) {
        free_http_request(conn->req);
    }
    free(conn);
}


void
http_close_cb(void* arg)
{
    http_connection_t *conn = (http_connection_t*)arg;
    LOG_DEBUG("http_close_cb");
    shutdown(conn->sockfd, SHUT_RD);
    free_http_connection(conn);
}


int
http_close_connection(http_connection_t* conn)
{
    int ret = close(conn->sockfd);
    free_http_connection(conn);
    return ret;
}

