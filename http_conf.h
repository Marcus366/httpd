#ifndef __HTTP_CONF_H__
#define __HTTP_CONF_H__

#include "http_log.h"

struct http_conf {
    enum http_log_level log_level;
};

struct http_conf* http_default_conf();
void http_read_conf(struct http_conf *conf, char *filename);

#endif
