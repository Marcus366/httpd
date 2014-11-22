#ifndef __HTTP_EVENT_H__
#define __HTTP_EVENT_H__


#include <sys/epoll.h>


#define HTTP_EVENT_IN  EPOLLIN
#define HTTP_EVENT_OUT EPOLLOUT
#define HTTP_EVENT_ET  EPOLLET


typedef struct http_event http_event_t;
typedef struct http_event_dispatcher http_event_dispatcher_t;

typedef void* http_event_data_t;
typedef void (*http_event_handler_t)(http_event_t *ev);


struct http_event {
    int                      fd;
    int                      type;
    http_event_data_t        data;
    http_event_handler_t     handler;

    http_event_dispatcher_t *dispatcher;
};


struct http_event_dispatcher {
    int epollfd;

    int maxevents;
    struct epoll_event *events;
};


http_event_t* http_event_create(int fd, int type, http_event_data_t data,
    http_event_handler_t handler);


http_event_dispatcher_t* http_event_dispatcher_create(int maxevents);

int http_event_dispatcher_add_event(http_event_dispatcher_t *dispatcher,
    http_event_t *event);
int http_event_dispatcher_del_event(http_event_dispatcher_t *dispatcher,
    http_event_t *event);

int http_event_dispatcher_poll(http_event_dispatcher_t *dispatcher);


#endif

