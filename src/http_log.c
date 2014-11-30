#include <stdio.h>
#include <stdarg.h>

#include "http_log.h"

enum http_log_level log_level;
char http_log_buf[LOG_BUFSIZE];

int
http_log_set_level(enum http_log_level level)
{
    return log_level = level;
}


int
http_log(enum http_log_level level, char *fmt, ...)
{
    if (level < log_level) {
        return LOG_IGN;
    }

    va_list args;
    va_start(args, fmt);
    vsnprintf(http_log_buf, LOG_BUFSIZE, fmt, args);
    va_end(args);

    fprintf(stderr, "%s", http_log_buf);

    return LOG_OK;
}

