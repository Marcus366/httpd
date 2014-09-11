#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "server.h"
#include "connect.h"
#include "request.h"

enum conn_state {
    CONN_IDLE,
    CONN_READ,
    CONN_WRITE
};

struct connect {
    int sockfd;
    enum conn_state state;
    struct request *req;
};

struct connect* new_connect(int sockfd);

int handle_new_connect(struct server* svc);
int handle_transfer(struct connect* conn);

#endif
