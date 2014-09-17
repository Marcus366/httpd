#include <stdlib.h>

#include "http_timer.h"

static struct http_timer *timer_head;

int http_timer_init() {
    timer_head = (struct http_timer*)malloc(sizeof(struct http_timer));
    timer_head->lprev = timer_head->lnext = timer_head;
    return 0;
}

struct http_timer* http_timer_create(double interval, http_timer_cb cb, void *arg, enum timer_type type)
{
    struct http_timer *timer = (struct http_timer*)malloc(sizeof(struct http_timer));
    timer->cb = cb;
    timer->arg = arg;
    timer->type = type;

    gettimeofday(&timer->trigger, NULL);
    timer->trigger.tv_sec += (long)interval;
    timer->trigger.tv_usec += (interval - (long)interval) * 1e6;
    if (timer->trigger.tv_usec > 1e6) {
        timer->trigger.tv_usec -= 1e6;
        timer->trigger.tv_sec += 1;
    }

    timer->lnext = timer_head->lnext;
    timer->lprev = timer_head;

    timer_head->lnext->lprev = timer;
    timer_head->lnext        = timer;

    return timer;
}

void http_timer_run()
{
    struct timeval now;
    gettimeofday(&now, NULL);

    struct http_timer *timer = timer_head->lnext;
    while (timer != timer_head) {
        if (timer->trigger.tv_sec < now.tv_sec
        || (timer->trigger.tv_sec == now.tv_sec && timer->trigger.tv_usec <= now.tv_usec))
        {
            timer->cb(timer->arg);
            if (timer->type == TIMER_ONCE) {
                struct http_timer *prev = timer->lprev;
                struct http_timer *next = timer->lnext;
                prev->lnext = next;
                next->lprev = prev;
                free(timer);
            } else if (timer->type == TIMER_CYCLE) {
            }
        }
        timer = timer->lnext;
    }
}
