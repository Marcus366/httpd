#ifndef __LOG_H__
#define __LOG_H__

//VERBOSE 37 white
//DEBUG   34 blue 
//INFO    36 green
//WARN    33 yellow
//ERROR   31 red

#define __DEBUG__

#ifdef __DEBUG__

#define LOG_VERBOSE(fmt, arg...)                             \
do {                                                          \
    http_log(ll_verbose, "\033[;37m[%s:%d][%s] " fmt "\n\033[0m", \
        __FILE__, __LINE__, __FUNCTION__, ##arg);             \
} while(0)

#define LOG_DEBUG(fmt, arg...)                               \
do {                                                          \
    http_log(ll_debug, "\033[;34m[%s:%d][%s] " fmt "\n\033[0m",   \
        __FILE__, __LINE__, __FUNCTION__, ##arg);             \
} while(0)

#define LOG_INFO(fmt, arg...)                                \
do {                                                          \
    http_log(ll_info, "\033[;36m[%s:%d][%s] " fmt "\n\033[0m",    \
        __FILE__, __LINE__, __FUNCTION__, ##arg);             \
} while(0)

#define LOG_WARN(fmt, arg...)                                \
do {                                                          \
    http_log(ll_warn, "\033[;33m[%s:%d][%s] " fmt "\n\033[0m",    \
        __FILE__, __LINE__, __FUNCTION__, ##arg);             \
} while(0)

#define LOG_ERROR(fmt, arg...)                               \
do {                                                          \
    http_log(ll_error, "\033[;31m[%s:%d][%s] " fmt "\n\033[0m",   \
        __FILE__, __LINE__, __FUNCTION__, ##arg);             \
} while(0)

#else

#define LOG_VERBOSE(fmt, arg...)
#define LOG_DEBUG(fmt, arg...)
#define LOG_INFO(fmt, arg...)
#define LOG_WARN(fmt, arg...)
#define LOG_ERROR(fmt, arg...)

#endif //__DEBUG__

#define LOG_BUFSIZE 1024

#define LOG_OK  0
#define LOG_IGN 1

enum http_log_level {
    ll_verbose,
    ll_debug,
    ll_info,
    ll_warn,
    ll_error
};

enum http_log_level log_level;

int http_log_set_level(enum http_log_level);
int http_log(enum http_log_level, char *fmt, ...);

#endif //__LOG_H__