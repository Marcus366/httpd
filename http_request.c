#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "http_request.h"
#include "string_utils.h"

static int http_parse_request_line(http_request_t *req, u_char *start, u_char *end);
static int http_parse_request_head(http_request_t *req, u_char *start, u_char *end);
static int http_parse_request_body(http_request_t *req, u_char *start, u_char *end);

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
    u_char *start = req->read_buf + req->check_idx;
    u_char *end   = req->read_buf + req->read_idx;

    while ((end = str_substr(start, end, "\r\n")) != NULL) {

      switch (req->major_state) {
      case PARSING_REQUEST_LINE:
          ret = http_parse_request_line(req, start, end);
          break;
      case PARSING_REQUEST_HEAD:
          break;
      case PARSING_REQUEST_BODY:
          break;
      }

      end += 2;
      req->check_idx += (end - start);
      start = req->read_buf + req->check_idx;

    }

    return ret;
}


int
http_parse_request_line(http_request_t *req, u_char *start, u_char *end)
{
    u_char* p;

    if ((p = str_substr(start, end, " ")) == NULL) {
        return -1;
    } else {
        req->method = (const char*)start;
        *p = '\0';
    }

    start = p + 1;
    if ((p = str_substr(start, end, " ")) == NULL) {
        return -1;
    } else {
        req->uri = (const char*)start;
        *p = '\0';
    }

    req->version = (const char*)(p + 1);
    *end = '\0';

    req->major_state = PARSING_REQUEST_HEAD;

    return 0;
}


int
http_parse_request_head(http_request_t *req, u_char *start, u_char *end)
{
    return 0;
}
