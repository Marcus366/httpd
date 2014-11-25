#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "http_request.h"
#include "http_log.h"
#include "http_mem.h"

static int http_parse_request_line(http_request_t *req, http_mem_t mem);
static int http_parse_request_head(http_request_t *req, http_mem_t mem);
static int http_parse_request_body(http_request_t *req, http_mem_t mem);


http_request_t*
new_http_request(size_t bufsize)
{
    http_request_t *req = (http_request_t*)malloc(sizeof(http_request_t));
    if (req != NULL) {
        req->major_state = PARSING_REQUEST_LINE;
        req->read_buf    = (u_char*)malloc(bufsize);
        req->buf_size    = bufsize;
        req->read_idx    = 0;
        req->check_idx   = 0;

        req->http_headers = http_headers_new();
    }
    return req;
}


void
free_http_request(http_request_t *req)
{
    if (req != NULL) {
        http_headers_free(req->http_headers);

        free(req->read_buf);
        free(req);
    }
}


ssize_t
http_recv_request(http_request_t *req, int sockfd)
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
            req->read_buf = (u_char*)realloc(req->read_buf, req->buf_size);
        }
    }
    return cnt;
}


int
http_parse_request(http_request_t *req)
{
    int ret = 0;
    http_mem_t token;
    http_mem_t mem = http_mem_create(req->read_buf + req->check_idx,
        req->read_idx - req->check_idx);

    do {
        mem = http_mem_create(req->read_buf + req->check_idx,
            req->read_idx - req->check_idx);
        token = http_mem_cut(mem, http_mem_create(CRLF, 2));
        if (http_mem_is_null(token) || ret != 0) {
            break;
        }

        switch (req->major_state) {
        case PARSING_REQUEST_LINE:
            ret = http_parse_request_line(req, token);
            break;
        case PARSING_REQUEST_HEAD:
            ret = http_parse_request_head(req, token);
            break;
        case PARSING_REQUEST_BODY:
            ret = http_parse_request_body(req, token);
            break;
        }

        req->check_idx += token.len;

    } while (1);
    return ret;
}


int
http_parse_request_line(http_request_t *req, http_mem_t mem)
{
    LOG_DEBUG("parse request line");
    http_mem_print(mem);

    req->method = http_mem_cut(mem, http_mem_create(SPACE, 1));
    if (http_mem_is_null(req->method)) {
        return -1;
    } else {
        mem.len  -= req->method.len;
        mem.base += req->method.len;
        req->method.base[--req->method.len] = 0;
    }
    http_mem_print(req->method);

    req->uri = http_mem_cut(mem, http_mem_create(SPACE, 1));
    if (http_mem_is_null(req->uri)) {
        return -1;
    } else {
        mem.len  -= req->uri.len;
        mem.base += req->uri.len;
        req->uri.base[--req->uri.len] = 0;
    }
    http_mem_print(req->uri);

    req->version = http_mem_cut(mem, http_mem_create(CRLF, 2));
    if (http_mem_is_null(req->version)) {
        return -1;
    } else {
        req->version.len -= 2;
        req->version.base[req->version.len] = 0;
    }
    http_mem_print(req->version);

    req->major_state = PARSING_REQUEST_HEAD;

    return 0;
}


int
http_parse_request_head(http_request_t *req, http_mem_t mem)
{
    LOG_DEBUG("parse request head");
    http_mem_print(mem);
    http_mem_t cut = http_mem_cut(mem, http_mem_create((u_char*)": ", 2));

    http_mem_t attr  = http_mem_create(mem.base, cut.len - 2);
    http_mem_t value = http_mem_create(mem.base + cut.len, mem.len - cut.len); 

    if (http_mem_equal(mem, value)) {
        return 1;
    }

    if (http_mem_is_null(attr) || http_mem_is_null(value)) {
        return -1;
    }

    http_header_set(req->http_headers, attr, value);

    return 0;
}


int
http_parse_request_body(http_request_t *req, http_mem_t mem)
{
    return 0;
}
