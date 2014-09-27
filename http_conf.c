#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "http_conf.h"

struct http_conf*
http_default_conf()
{
    struct http_conf *conf = (struct http_conf*)malloc(sizeof(struct http_conf));
    conf->log_level = ll_verbose;
    return conf;
}

void
http_read_conf(struct http_conf *conf, char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        LOG_WARN("config file[%s] not found", filename);
    }

    char token[64], flag[64];
    fscanf(file, "%s: %s", token, flag);
    if (strncasecmp(token, "log_level", 64) == 0) {
        if (strncasecmp(flag, "verbose", 64) == 0) {
            conf->log_level = ll_verbose;
        } else if (strncasecmp(flag, "debug", 64) == 0) {
            conf->log_level = ll_debug;
        }  else if (strncasecmp(flag, "info", 64) == 0) {
            conf->log_level = ll_info;
        } else if (strncasecmp(flag, "warn", 64) == 0) {
            conf->log_level = ll_warn;
        } else if (strncasecmp(flag, "error", 64) == 0) {
            conf->log_level = ll_error;
        } else {
            LOG_WARN("invalid loglevel flag[%s] of config file[%s]", flag, filename);
        }
    }

    fclose(file);
}
