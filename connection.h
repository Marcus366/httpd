#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "request.h"

struct connection {
    int sockfd;
    struct request req;
};

#endif
