#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "http_req.h"

struct http_req* new_http_req(size_t bufsize)
{
    struct http_req *req = (struct http_req*)malloc(sizeof(struct http_req));
    if (req != NULL) {
        req->state = REQ_PARSE_METHOD_BEGIN;
        req->read_buf = (char*)malloc(bufsize);
        req->buf_size = bufsize;
        req->read_idx = 0;
        req->check_idx = 0;
    }
    return req;
}

void free_http_req(struct http_req *req)
{
    if (req != NULL) {
        free(req->read_buf);
        free(req);
    }
}

ssize_t http_read_req(struct http_req *req, int sockfd)
{
    ssize_t cnt = 0;

    for (;;) {
        ssize_t nread, free_size = req->buf_size - req->read_idx;
        nread = read(sockfd, req->read_buf + req->read_idx, free_size);
        if (nread == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                perror("read");
                return 0;
            }
        }
        cnt += nread;
        req->read_idx += nread;
        if (nread < free_size) {
            break;
        }
        if (req->read_idx == req->buf_size) {
            req->buf_size = req->buf_size << 1;;
            req->read_buf = (char*)realloc(req->read_buf, req->buf_size);
        }
    }
    return cnt;
}

int http_parse_req(struct http_req *req)
{
    char *c;
    while (req->check_idx < req->read_idx) {
        c = req->read_buf + req->check_idx++;
        switch (req->state) {
            case REQ_PARSE_METHOD_BEGIN:
                req->method = c;
                req->state = REQ_PARSING_METHOD;
                break;
            case REQ_PARSING_METHOD:
                if (*c == ' ') {
                    *c = 0;
                    req->state = REQ_PARSE_URI_BEGIN;
                }
                break;
            case REQ_PARSE_URI_BEGIN:
                req->uri = c;
                req->state = REQ_PARSING_URI;
                break;
            case REQ_PARSING_URI:
                if (*c == ' ') {
                    *c = 0;
                    req->state = REQ_PARSE_VERSION_BEGIN;
                }
                break;
            case REQ_PARSE_VERSION_BEGIN:
                req->version = c;
                req->state = REQ_PARSING_VERSION;
                break;
            case REQ_PARSING_VERSION:
                if (*c == '\r') {
                    *c = 0;
                    req->state = REQ_CL;
                }
                break;
            case REQ_CL:
                if (*c == '\n') {
                    req->state = REQ_CLRF;
                }
                break;
            case REQ_CLRF:
                if (*c == '\r') {
                    req->state = REQ_CLRFCL;
                }
                break;
            case REQ_CLRFCL:
                if (*c == '\n') {
                    req->state = REQ_PARSE_END;
                }
                return 1;
            default:
                break;
        }
    }
    return 0;
}
