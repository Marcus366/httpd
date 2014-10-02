#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <errno.h>

#include "http_req.h"
#include "http_res.h"
#include "http_log.h"

#include "http_fcache.h"

static inline
int copy_and_mv(char **out, char* in)
{
    while (*in != 0) {
        *((*out)++) = *in++;
    }
    **out = 0;
    return 1;
}

struct http_res*
new_http_res()
{
    struct http_res *res = (struct http_res*)malloc(sizeof(struct http_res));
    if (res != NULL) {
        res->state    = REQ_GEN_BEGIN;
        res->send_buf = NULL;
        res->buf_size = 0;
        res->buf_len  = 0;
        res->send_idx = 0;

        res->file     = NULL;
        res->offset   = 0;
    }
    return res;
}

void
free_http_res(struct http_res *res)
{
    if (res != NULL) {
        free(res->send_buf);
        free(res);
    }
}

int
http_gen_res(struct http_res *res, struct http_req *req)
{
    struct http_fcache_file *file;

    file = http_fcache_getfile(fcache, req->uri + 1);
    if (file == NULL) {
        if ((file = http_fcache_putfile(fcache, req->uri + 1)) == NULL) {
            return -1;
        }
    }
    res->send_buf = (char*)malloc((int)file->stat.st_size + 1024);
    res->buf_size = (int)file->stat.st_size + 1024;
    char *buf = res->send_buf;
    copy_and_mv(&buf, req->version);
    copy_and_mv(&buf, " ");
    copy_and_mv(&buf, HTTP_OK);
    copy_and_mv(&buf, "\r\n");
    copy_and_mv(&buf, "Server: ZZPServer\r\nDate: Sat, 31 Dec 2014 23:59:59 GMT\r\nContent-Type: text/html\r\n");
    char contentlen[64];
    sprintf(contentlen, "Content-Length: %d\r\n", (int)file->stat.st_size);
    copy_and_mv(&buf, contentlen);
    res->buf_len = strlen(res->send_buf);

    res->file = file;

    res->state = REQ_SEND_HEADER;
    return 0;
}

enum send_state
http_send_res(struct http_res *res, int sockfd)
{
    if (res->state == REQ_SEND_HEADER) {
        for (;;) {
            ssize_t nwrite, rest = res->buf_len - res->send_idx;
            LOG_VERBOSE("write %d bytes", rest);
            nwrite = write(sockfd, res->send_buf + res->send_idx, rest);
            if (nwrite == -1) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN) {
                    return SEND_BLOCK;
                } else {
                    LOG_WARN("write: %s", strerror(errno));
                    return SEND_ERROR;
                }
            }
            res->send_idx += nwrite;
            if (nwrite == 0 || nwrite == rest) {
                res->state = REQ_SEND_BODY;
                goto sendbody;
            } else if (nwrite < rest) {
                return SEND_BLOCK;
            }
        }
    } else if (res->state == REQ_SEND_BODY) {
sendbody:
        for (;;) {
            ssize_t nwrite, rest = res->file->stat.st_size - res->offset;
            nwrite = sendfile(sockfd, res->file->fd, &res->offset, rest);
            if (nwrite == -1) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN) {
                    return SEND_BLOCK;
                } else {
                    LOG_WARN("sendfile: %s", strerror(errno));
                    return SEND_ERROR;
                }
            }
            if (res->offset == res->file->stat.st_size) {
                return SEND_FINISH;
            } else {
                return SEND_BLOCK;
            }
        }
    } else {
        LOG_WARN("Invalid response state");
        return SEND_FINISH;
    }
}
