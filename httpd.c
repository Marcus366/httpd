#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>

#include "http_srv.h"
#include "http_config.h"
#include "http_log.h"

http_config *config = NULL;

CONFIG_VARIABLE(int, port);
CONFIG_INTERFACE(void, set_port) {
    port = lua_tointeger(L, -1);
    lua_pop(L, 1);
}

void
sighup(int signo) {
    LOG_VERBOSE("sighup");
    http_reload_config(config, "httpd.conf");
}

int main(int argc, char** argv)
{
    int i;
    pid_t pid;
    FILE *file;
    struct sigaction sa;

    for (i = 1; i < argc; ++i) {
        if (strcmp("-r", argv[i]) == 0) {
            if ((file = fopen("logs/httpd.pid", "r")) == NULL) {
                LOG_ERROR("open httpd.pid: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            (void)fscanf(file, "%d", &pid);
            fclose(file);

            if (kill(pid, SIGHUP) == -1) {
                LOG_ERROR("send SIGHUP failed: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            LOG_INFO("send sighup signal");
            exit(EXIT_SUCCESS);
        }
    }

    sa.sa_handler = sighup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        LOG_ERROR("sigaction: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    http_log_set_level(ll_verbose);

    config = http_create_config();
    http_reigster_config_directive(config, "set_port", (config_handler)set_port);
    http_load_config(config, "httpd.conf");
    LOG_VERBOSE("%d", port);

    pid = getpid();
    if ((file = fopen("logs/httpd.pid", "w")) == NULL) {
        LOG_ERROR("open httpd.pid: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%ld", (long)pid);
    fclose(file);

    struct http_srv* srv = new_http_srv(port);
    if (srv != NULL) {
        serve(srv);
    }
    //never return
    exit(-1);
}
