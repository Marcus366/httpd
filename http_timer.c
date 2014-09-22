#include <stdlib.h>

#include "http_timer.h"

static struct http_timer *timer_head;

int http_timer_init()
{
    timer_head = (struct http_timer*)malloc(sizeof(struct http_timer));
    timer_head->lprev = timer_head->lnext = timer_head;
    return 0;
}

int http_timer_minimal_timeout()
{
    int min = 0x8FFFFFFF;
    struct http_timer *timer = timer_head->lnext;

    while (timer != timer_head) {
        if (timer->timeout < min) {
            min = timer->timeout;
        }
    }

    return min;
}

struct http_timer* http_timer_create(int msec, http_timer_cb cb, void *arg, enum timer_type type)
{
    struct http_timer *timer = (struct http_timer*)malloc(sizeof(struct http_timer));
    timer->cb       = cb;
    timer->arg      = arg;
    timer->type     = type;
    timer->interval = msec;
    timer->timeout  = msec;

    timer->lnext = timer_head->lnext;
    timer->lprev = timer_head;

    timer_head->lnext->lprev = timer;
    timer_head->lnext        = timer;

    return timer;
}

void http_timer_run(struct timeval *last, struct timeval *now)
{
    unsigned long delta = (now->tv_sec - last->tv_sec) * 1e3 + (now->tv_usec - last->tv_usec) / 1e3;

    struct http_timer *timer = timer_head->lnext;
    while (timer != timer_head) {
        timer->timeout -= delta;
        if (timer->timeout <= 0) {
            timer->cb(timer->arg);
            if (timer->type == TIMER_ONCE) {
                struct http_timer *prev = timer->lprev;
                struct http_timer *next = timer->lnext;
                prev->lnext = next;
                next->lprev = prev;
                free(timer);
                timer = next;
            } else if (timer->type == TIMER_CYCLE) {
                timer->timeout = timer->interval;
                timer = timer->lnext;
            }
        }
    }
}
