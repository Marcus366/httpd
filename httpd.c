#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "httpd.h"
#include "http_log.h"
#include "http_res.h"
#include "http_event.h"
#include "http_server.h"
#include "http_config.h"
#include "http_fcache.h"
#include "http_connection.h"


CONFIG_VARIABLE(int, port);
CONFIG_INTERFACE(void, set_port) {
    port = lua_tointeger(L, -1);
    lua_pop(L, 1);
}

static int http_sighup;
static void http__sighup();

void
http__sighup(int signo) {
    (void) signo;
    http_sighup = 1;
}


static int http__arguments_init(int argc, char **argv);
static int http__signals_init();
static int http__config_init(http_looper_t *looper);
static int http__socket_init(http_looper_t *looper);

static void http__start_master_loop(http_looper_t *looper);
static void http__start_worker_loop(http_looper_t *looper);

static void http__accept(http_event_t *ev);
static void http__read(http_event_t *ev);
static void http__write(http_event_t *ev);


int
main(int argc, char** argv)
{
    int i;
    pid_t pid;
    FILE *file;

    http_looper_t *looper = (http_looper_t*)malloc(sizeof(http_looper_t));

    if (http__arguments_init(argc, argv) != 0) {
        exit(EXIT_FAILURE);
    }

    if (http__signals_init() != 0) {
        exit(EXIT_FAILURE);
    }

    http_log_set_level(ll_verbose);

    if (http__config_init(looper) != 0) {
        exit(EXIT_FAILURE);
    }

    if (http__socket_init(looper) != 0) {
        exit(EXIT_FAILURE);
    }

    pid = getpid();
    if ((file = fopen("/etc/httpd/httpd.pid", "w")) == NULL) {
        LOG_ERROR("open httpd.pid: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%ld", (long)pid);
    fclose(file);

    /*
    struct http_srv* srv = new_http_srv(port);
    if (srv != NULL) {
        serve(srv);
    }
    */

    for (i = 0; i < 0; ++i) {
        pid = fork();
        if (pid < 0) {
            LOG_ERROR("fork error");
        } else if (pid == 0) {
            http__start_worker_loop(looper);
        }
    }

    http__start_worker_loop(looper);
    //http__start_master_loop(looper);

    /* Never return. */
    /* Make valgrind happy. */
    http_free_config(looper->config);

    exit(-1);
}


int
http__arguments_init(int argc, char **argv)
{
    int i;
    pid_t pid;
    FILE *file;

    for (i = 1; i < argc; ++i) {
        if (strcmp("-r", argv[i]) == 0) {
            if ((file = fopen("/etc/httpd/httpd.pid", "r")) == NULL) {
                LOG_ERROR("open httpd.pid: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            if (fscanf(file, "%d", &pid) != 1) {
                LOG_ERROR("file httpd.pid formatted error");
                exit(EXIT_FAILURE);
            }
            fclose(file);

            if (kill(pid, SIGHUP) == -1) {
                LOG_ERROR("send SIGHUP failed: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            LOG_INFO("send sighup signal");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}


int
http__signals_init()
{
    struct sigaction sa;

    sa.sa_handler = http__sighup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        LOG_ERROR("sigaction: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}


int
http__config_init(http_looper_t *looper)
{
    looper->config = http_create_config();
    http_reigster_config_directive(looper->config,
        "set_port", (config_handler)set_port);
    http_load_config(looper->config, "/etc/httpd/httpd.conf");
    LOG_VERBOSE("%d", port);

    return 0;
}


int
http__socket_init(http_looper_t *looper)
{
    int listenfd;
    struct sockaddr_in addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        LOG_ERROR("socket: %s", strerror(errno));
        return -1;
    }

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        LOG_ERROR("bind: %s", strerror(errno));
        return -1;
    }

    if (listen(listenfd, 1024) != 0) {
        LOG_ERROR("listen %s", strerror(errno));
        return -1;
    }

    /* TODO:
     * Here is not the final edtion code,
     * It should be listen all the port needed.
     */
    looper->listening =
        (http_listen_socket_t*)malloc(sizeof(http_listen_socket_t));
    looper->listening->port = 80;
    looper->listening->fd = listenfd;
    looper->listening->next = NULL;
    int fl = fcntl(listenfd, F_GETFL);
    if (fl == -1) {
        perror("getfl");
        close(listenfd);
        return -1;
    }
    if (fcntl(listenfd, F_SETFL, fl | O_NONBLOCK) == -1) {
        perror("setfl");
        close(listenfd);
        return -1;
    }

    /*
     * FIXME:
     * I don't know where to gracefully place the file cache.
     */
    fcache = http_fcache_create(10);

    return 0;
}


void
http__start_master_loop(http_looper_t *looper)
{
    for(;;) {
        if (http_sighup == 1) {
            http_sighup = 0;
            LOG_DEBUG("sighup");
            http_reload_config(looper->config, "httpd.conf");
        }
    }
}


void
http__start_worker_loop(http_looper_t *looper)
{
    /* TODO:
     * Block signals here.
     */

    http_event_t *event;
    http_listen_socket_t *listening;
    looper->dispatcher = http_event_dispatcher_create(1024);

    listening = looper->listening;
    while (listening != NULL) {
        event = http_event_create(listening->fd, HTTP_EVENT_IN,
            (http_event_data_t)listening, (http_event_handler_t)http__accept);
        http_event_dispatcher_add_event(looper->dispatcher, event);

        listening = listening->next;
    }

    http_event_dispatcher_poll(looper->dispatcher);
}


void
http__accept(http_event_t *ev)
{
    int connfd;
    socklen_t conn_len;
    struct sockaddr_in conn_addr;
    http_event_t *event;
    http_connection_t *conn;
    http_listen_socket_t *listening;

    conn_len = 0;
    listening = (http_listen_socket_t*)ev->data;
    for (;;) {
        memset(&conn_addr, 0, sizeof(conn_addr));
        conn_len = sizeof(conn_addr);

        if ((connfd = accept(listening->fd, (struct sockaddr*)&conn_addr, &conn_len)) == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                perror("accept");
                return ;
            }
        }
        int fl = fcntl(connfd, F_GETFL);
        if (fl == -1) {
            perror("getfl");
            close(connfd);
            return ;
        }
        if (fcntl(connfd, F_SETFL, fl | O_NONBLOCK) == -1) {
            perror("setfl");
            close(connfd);
            return ;
        }

        conn = new_http_connection(connfd);
        event = http_event_create(connfd, HTTP_EVENT_IN | HTTP_EVENT_ET,
            (http_event_data_t)conn, (http_event_handler_t)http__read);
        http_event_dispatcher_add_event(ev->dispatcher, event);

        LOG_INFO("accept address %s, sockfd:%d, uuid:%llu", inet_ntoa(conn_addr.sin_addr), conn->sockfd, conn->uuid);
    }
}


void
http__read(http_event_t *ev)
{
    int connfd;
    http_connection_t *conn;

    conn = (http_connection_t*)ev->data;
    connfd = conn->sockfd;
    if (conn->req == NULL) {
        conn->req = new_http_request(1024);
    }

    http_recv_request(conn->req, conn->sockfd);

    if (http_parse_request(conn->req)) {
        LOG_DEBUG("parse request: method:%s uri:%s version:%s",
                conn->req->method.base, conn->req->uri.base, conn->req->version.base);
        ev->type = HTTP_EVENT_OUT | HTTP_EVENT_ET;
        ev->handler = http__write;
        http_event_dispatcher_mod_event(ev->dispatcher, ev);
        conn->state = CONN_WRITE;
    }

    return ;
}


void
http__write(http_event_t *ev)
{
    http_connection_t *conn;

    conn = (http_connection_t*)ev->data;
    if (conn->res == NULL) {
        conn->res = new_http_res();
        if (http_gen_res(conn->res, conn->req) == -1) {
            http_close_connection(conn);
            return ;
        }
    }

    if (http_send_res(conn->res, conn->sockfd) == SEND_FINISH) {
        LOG_VERBOSE("SEND_FINISH");
        SET_CONN_STATE(conn, CONN_WAIT_CLOSE);
        http_close_connection(conn);
        //shutdown(conn->sockfd, SHUT_WR);
        //http_timer_create(1e6, http_close_cb, conn, TIMER_ONCE);
    }
    return ;
}

