#ifndef __HTTP_TIMER_H__
#define __HTTP_TIMER_H__

#include <sys/time.h>

enum timer_type {
    TIMER_ONCE,
    TIMER_CYCLE
};

typedef void (*http_timer_cb)(void *arg);

struct http_timer {
    http_timer_cb        cb;
    void                *arg;
    enum timer_type      type;
    long                 interval;
    long                 timeout;
    //struct http_timer   *hprev;
    //struct http_timer   *hnext;
    struct http_timer   *lprev;
    struct http_timer   *lnext;
};

int http_timer_init();
void http_timer_run(struct timeval last, struct timeval now);

/* Create a new timer and add it to scheduler */
struct http_timer* http_timer_create(int usec, http_timer_cb cb, void *arg, enum timer_type type);

/*
 * Remove a current timer for scheduler list.
 * Return next timer of current timer.
 */
struct http_timer* http_timer_cancel(struct http_timer *timer);

int http_timer_minimal_timeout();

static void
http_timer_trigger(struct http_timer *timer)
{
   timer->cb(timer->arg);
}

#endif
