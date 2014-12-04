#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "http_server.h"
#include "http_connection.h"
#include "http_timer.h"
#include "http_log.h"
#include "http_fcache.h"


http_server_t*
new_http_server(int listenfd)
{
    http_server_t* svc = (http_server_t*)malloc(sizeof(http_server_t));
    svc->listenfd = listenfd;

    gettimeofday(&svc->now, NULL);
    http_timer_init();


    return svc;
}


void serve(http_server_t* svc)
{
}

