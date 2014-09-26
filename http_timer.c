#include <stdio.h>
#include <stdlib.h>

#include "http_timer.h"

static struct http_timer *timer_head;
static int timer_count;

int http_timer_init()
{
    timer_head = (struct http_timer*)malloc(sizeof(struct http_timer));
    timer_head->lprev = timer_head->lnext = timer_head;
    return 0;
}

int http_timer_minimal_timeout()
{
    long min = 0x7FFFFFFF;
    struct http_timer *timer = timer_head->lnext;

    if (timer_head == timer) {
        return -1;
    }

    while (timer != timer_head) {
        if (timer->timeout < min) {
            min = timer->timeout;
        }
        timer = timer->lnext;
    }
    printf("minimal timeout:%ld\n", (long)(min * 1e-3));

    return min * 1e-3;
}

struct http_timer* http_timer_create(int usec, http_timer_cb cb, void *arg, enum timer_type type)
{
    struct http_timer *timer = (struct http_timer*)malloc(sizeof(struct http_timer));
    timer->cb       = cb;
    timer->arg      = arg;
    timer->type     = type;
    timer->interval = usec;
    timer->timeout  = usec;

    timer->lnext = timer_head->lnext;
    timer->lprev = timer_head;

    timer_head->lnext->lprev = timer;
    timer_head->lnext        = timer;

    ++timer_count;

    return timer;
}

struct http_timer* http_timer_cancel(struct http_timer *timer)
{
    struct http_timer *prev = timer->lprev;
    struct http_timer *next = timer->lnext;
    prev->lnext = next;
    next->lprev = prev;
    free(timer);

    --timer_count;

    return next;
}

void http_timer_run(struct timeval last, struct timeval now)
{
    unsigned long delta = (now.tv_sec - last.tv_sec) * 1e6 + (now.tv_usec - last.tv_usec);

    struct http_timer *timer = timer_head->lnext;
    while (timer != timer_head) {
        timer->timeout -= delta;
        if (timer->timeout <= 0) {
            http_timer_trigger(timer);
            if (timer->type == TIMER_ONCE) {
                timer = http_timer_cancel(timer);
            } else if (timer->type == TIMER_CYCLE) {
                timer->timeout = timer->interval;
                timer = timer->lnext;
            } else {
                printf("invalid type of timer\n");
            }
        } else {
            timer = timer->lnext;
        }
    }
}
