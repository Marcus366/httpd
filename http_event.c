#include <stdlib.h>
#include "http_event.h"


http_event_t*
http_event_create(int fd, int type, http_event_data_t data,
    http_event_handler_t handler)
{
    http_event_t *event;

    event = (http_event_t*)malloc(sizeof(http_event_t));

    event->fd      = fd;
    event->type    = type;
    event->data    = data;
    event->handler = handler;

    event->dispatcher = NULL;

    return event;
}


http_event_dispatcher_t*
http_event_dispatcher_create(int maxevents)
{
    http_event_dispatcher_t *dispatcher;

    dispatcher =
        (http_event_dispatcher_t*)malloc(sizeof(http_event_dispatcher_t));

    dispatcher->maxevents = maxevents;
    dispatcher->events =
        (struct epoll_event*)malloc(sizeof(struct epoll_event) * maxevents);

    if ((dispatcher->epollfd = epoll_create(maxevents)) == -1) {
        free(dispatcher->events);
        free(dispatcher);
        return NULL;
    }

    return dispatcher;
}


int
http_event_dispatcher_add_event(http_event_dispatcher_t *dispatcher,
    http_event_t *event)
{
    int epollfd;
    struct epoll_event ev;

    epollfd = dispatcher->epollfd;
    ev.data.ptr = event;
    ev.events = event->type;

    event->dispatcher = dispatcher;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, event->fd, &ev) == -1) {
        return -1;
    }

    return 0;
}


int
http_event_dispatcher_del_event(http_event_dispatcher_t *dispatcher,
    http_event_t *event)
{
    int epollfd;
    struct epoll_event ev;

    epollfd = dispatcher->epollfd;

    event->dispatcher = NULL;
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, event->fd, &ev) == -1) {
        return -1;
    }

    return 0;
}


int
http_event_dispatcher_mod_event(http_event_dispatcher_t *dispatcher,
    http_event_t *event)
{
    int epollfd;
    struct epoll_event ev;

    epollfd = dispatcher->epollfd;
    ev.data.ptr = event;
    ev.events = event->type;

    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, event->fd, &ev) == -1) {
        return -1;
    }

    return 0;
}


int
http_event_dispatcher_poll(http_event_dispatcher_t *dispatcher)
{
    http_event_t *event;
    struct epoll_event ev;
    int i, nfds, epollfd;

    epollfd = dispatcher->epollfd;
    
    for (;;) {
        nfds = epoll_wait(epollfd, dispatcher->events, dispatcher->maxevents, -1);
        for (i = 0; i < nfds; ++i) {
            ev = dispatcher->events[i];
            event = (http_event_t*)ev.data.ptr;

            event->handler(event);
        }
    }

    return 0;
}

