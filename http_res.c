#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "http_req.h"
#include "http_res.h"

static inline int copy_and_mv(char **out, char* in)
{
    while (*in != 0) {
        *((*out)++) = *in++;
    }
    **out = 0;
    return 1;
}

struct http_res* new_http_res(size_t bufsize)
{
    struct http_res *res = (struct http_res*)malloc(sizeof(struct http_res));
    if (res != NULL) {
        res->state    = REQ_GEN_BEGIN;
        res->send_buf = (char*)malloc(bufsize);
        res->buf_size = bufsize;
        res->buf_len = 0;
        res->send_idx = 0;
    }
    return res;
}

int http_gen_res(struct http_res *res, struct http_req *req)
{
    char *buf = res->send_buf;
    /*int fd = open(req->uri + 1, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 0;
    }*/
    copy_and_mv(&buf, req->version);
    copy_and_mv(&buf, " ");
    copy_and_mv(&buf, HTTP_OK);
    copy_and_mv(&buf, "\r\n");
    copy_and_mv(&buf, "Server: Nginx\r\nDate: Sat, 31 Dec 2014 23:59:59 GMT\r\nContent-Type: text/html\r\nContent-Length: 122\r\n\r\n<html>\r\n<head>\r\n<title>Wrox Homepage</title>\r\n</head>\r\n<body>\r\n</bod>\r\n</html>\r\n");

    res->buf_len = strlen(res->send_buf);
    return 0;
}

enum send_state http_send_res(struct http_res *res, int sockfd)
{
    for (;;) {
        ssize_t nwrite, free = res->buf_len - res->send_idx;
        printf("write %d bytes\n", free);
        nwrite = write(sockfd, res->send_buf + res->send_idx, free);
        if (nwrite == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                return SEND_BLOCK;
            } else {
                perror("write");
                return SEND_ERROR;
            }
        }
        res->send_idx += nwrite;
        if (nwrite == 0) {
            return SEND_FINISH;
        } else if (nwrite < free) {
            return SEND_BLOCK;
        }
    }
}
