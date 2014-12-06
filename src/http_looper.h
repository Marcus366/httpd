#ifndef __HTTP_LOOPER_H__
#define __HTTP_LOOPER_H__


#include "http_event.h"
#include "http_config.h"
#include "http_listen_socket.h"


typedef struct http_lopper {
    http_config             *config;
    http_listen_socket_t    *listening;

    http_event_dispatcher_t *dispatcher;
} http_looper_t;


#endif

