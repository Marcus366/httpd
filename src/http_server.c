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
    /*
    int nfds;
    struct epoll_event ev, events[1024];

    
    if (listen(svc->listenfd, 1024) != 0) {
        LOG_ERROR("listen: %s", strerror(errno));
        return;
    }
    
    int fl = fcntl(svc->listenfd, F_GETFL);
    if (fl == -1) {
        LOG_ERROR("getfl: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (fcntl(svc->listenfd, F_SETFL, fl | O_NONBLOCK) == -1) {
        LOG_ERROR("setfl: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = svc->listenfd;
    if (epoll_ctl(svc->epollfd, EPOLL_CTL_ADD, svc->listenfd, &ev) == -1) {
        LOG_ERROR("epoll_ctl: listen_sock: %s", strerror(errno));
        return;
    }
    */

    /*
    if (daemon(1, 1) == -1) {
        perror("deamon");
        return;
    }
    */

    /*
    for (;;) {
        LOG_VERBOSE("a more for loop");

        struct timeval now;
        gettimeofday(&now, NULL);
        http_timer_run(svc->now, now);
        svc->now = now;
    */
        /* FIXME Issue #1
         * It is strange to find that it trigger many time main loop,
         * if this function return 0.
         * Expected: It return 0 so that epoll_wait return without blocking.
         *           In such condition nfds may be 0 too and the control flow
         *           will go to next main loop. Then it call http_timer_run which
         *           trigger all timer and remove it.
         * Actually: It seem that http_timer_run have NOT trigger the timer so
         *           run mainloop many times.
         */
        /*
        int timeout = http_timer_minimal_timeout();
        nfds = epoll_wait(svc->epollfd, events, 1024, timeout);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }
            LOG_ERROR("epoll_wait: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == svc->listenfd) {
                handle_new_connection(svc);
            } else {
                http_connection_t *conn = (http_connection_t*)events[i].data.ptr;
                if (conn->state == CONN_READ) {
                    handle_read(conn);
                } else if (conn->state == CONN_WRITE) {
                    handle_write(conn);
                } else {
                    LOG_WARN("Invalid connect state:%d of uuid:%llu", (int)conn->state, conn->uuid);
                    if (conn->state != CONN_WAIT_CLOSE) {
                        http_close_connection(conn);
                    }
                }
            } 
        }
    }
    */
}
