#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "http_srv.h"
#include "http_conf.h"
#include "http_log.h"

static struct http_conf *conf = NULL;

void
sighup(int signo) {
    LOG_VERBOSE("sighup");
    http_read_conf(conf, "httpd.conf");
}

int main(int argc, char** argv)
{
    struct sigaction sa;
    sa.sa_handler = sighup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("sigaction");
        exit(-1);
    }

    conf = http_default_conf();
    http_read_conf(conf, "httpd.conf");

    struct http_srv* srv = new_http_srv(80);
    if (srv != NULL) {
        serve(srv);
    }
    //never return
    exit(-1);
}
