#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/uio.h>
#include <sys/sendfile.h>

#include "http_connection.h"
#include "http_request.h"
#include "http_log.h"
#include "http_mem.h"
#include "http_fcache.h"


static int http_parse_request_line(http_request_t *req, http_mem_t mem);
static int http_parse_request_head(http_request_t *req, http_mem_t mem);
static int http_parse_request_body(http_request_t *req, http_mem_t mem);


http_request_t*
new_http_request(http_connection_t *conn)
{
    http_request_t *req = (http_request_t*)malloc(sizeof(http_request_t));
    if (req != NULL) {
        req->conn        = conn;

        req->major_state = PARSING_REQUEST_LINE;
        req->read_buf    = (u_char*)malloc(1024);
        req->read_last   = req->read_buf + 1024;

        req->read_pos = req->check_pos = req->read_buf;

        req->headers_in  = http_headers_new();
        req->headers_out = NULL;
    }
    return req;
}


void
free_http_request(http_request_t *req)
{
    if (req != NULL) {
        http_headers_free(req->headers_in);

        free(req->read_buf);
        free(req);
    }
}


ssize_t
http_recv_request(http_request_t *req, int sockfd)
{
    ssize_t cnt = 0;

    for (;;) {
        ssize_t nread, free_size = req->read_last - req->read_pos;
        nread = read(sockfd, req->read_pos, free_size);
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
        req->read_pos += nread;
        if (nread < free_size) {
            break;
        }
        if (req->read_pos == req->read_last) {
            req->read_buf = (u_char*)realloc(req->read_buf,
                (req->read_last - req->read_buf) * 2);
        }
    }
    return cnt;
}


int
http_parse_request(http_request_t *req)
{
    int ret = 0;
    http_mem_t token;
    http_mem_t mem;

    do {
        mem = http_mem_create(req->check_pos,
            req->read_pos - req->check_pos);
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

        req->check_pos += token.len;
    } while (1);

    if (ret == 1) {
        req->major_state = BUILDING_RESPONSE;
    }

    return ret;
}


int
http_parse_request_line(http_request_t *req, http_mem_t mem)
{
    req->method = http_mem_cut(mem, http_mem_create(SPACE, 1));
    if (http_mem_is_null(req->method)) {
        return -1;
    } else {
        mem.len  -= req->method.len;
        mem.base += req->method.len;
        req->method.base[--req->method.len] = 0;
    }

    req->uri = http_mem_cut(mem, http_mem_create(SPACE, 1));
    if (http_mem_is_null(req->uri)) {
        return -1;
    } else {
        mem.len  -= req->uri.len;
        mem.base += req->uri.len;
        req->uri.base[--req->uri.len] = 0;
    }

    req->version = http_mem_cut(mem, http_mem_create(CRLF, 2));
    if (http_mem_is_null(req->version)) {
        return -1;
    } else {
        req->version.len -= 2;
        req->version.base[req->version.len] = 0;
    }

    req->major_state = PARSING_REQUEST_HEAD;

    return 0;
}


int
http_parse_request_head(http_request_t *req, http_mem_t mem)
{
    http_mem_t cut = http_mem_cut(mem, http_mem_create((u_char*)": ", 2));

    http_mem_t attr  = http_mem_create(mem.base, cut.len - 2);
    http_mem_t value = http_mem_create(mem.base + cut.len, mem.len - cut.len); 

    if (http_mem_equal(mem, value)) {
        return 1;
    }

    if (http_mem_is_null(attr) || http_mem_is_null(value)) {
        return -1;
    }

    http_header_set(req->headers_in, attr, value);

    return 0;
}


int
http_parse_request_body(http_request_t *req, http_mem_t mem)
{
    return 0;
}


char *global_buf = (char*)"HTTP/1.1 200 OK\r\nServer: ZZPServer\r\nDate: Sat, 31 Dec 2014 23:59:59 GMT\r\nContent-Type: text/html\r\n";


int
http_build_response(http_request_t *req)
{
    struct http_fcache_file *file;
    http_chain_t *chain;

    file = http_fcache_getfile(fcache, (const char*)req->uri.base + 1);
    if (file == NULL) {
        if ((file = http_fcache_putfile(fcache, (const char*)req->uri.base + 1)) == NULL) {
            return -1;
        }
    }

    chain = (http_chain_t*)malloc(sizeof(http_chain_t));
    chain->type = MEMORY_CHAIN;
    chain->data.mem = http_mem_create((u_char*)global_buf, strlen(global_buf));
    chain->offset.mem_off = 0;
    req->out_chain = chain;

    u_char *contentlen = (u_char*)malloc(64);
    chain = (http_chain_t*)malloc(sizeof(http_chain_t));
    sprintf((char*)contentlen, "Content-Length: %d\r\n\r\n", (int)file->stat.st_size);
    chain->type = MEMORY_CHAIN;
    chain->data.mem = http_mem_create(contentlen, strlen((char*)contentlen));
    chain->offset.mem_off = 0;
    req->out_chain->next = chain;

    chain = (http_chain_t*)malloc(sizeof(http_chain_t));
    chain->type = SENDFILE_CHAIN;
    chain->data.sendfile.fd = file->fd;
    chain->data.sendfile.size = file->stat.st_size;
    chain->offset.file_off = 0;
    chain->next = NULL;
    req->out_chain->next->next = chain;
    
    req->major_state = SENDING_RESPONSE;

    return 0;
}


int
http_send_response(http_request_t *req)
{
    int sockfd;
    http_chain_t *tmp, *chain;

    sockfd = req->conn->sockfd;
    chain = req->out_chain;

sendloop:
    if (chain == NULL) {
        LOG_DEBUG("send all the msg");
        return 1;
    }

    if (chain->type == SENDFILE_CHAIN) {
        for (;;) {
            ssize_t nwrite, rest;
            rest = chain->data.sendfile.size - chain->offset.file_off;
            nwrite = sendfile(sockfd, chain->data.sendfile.fd
                , &chain->offset.file_off, rest);
            LOG_DEBUG("offset:%d rest:%d have written:%d",
                (int)chain->offset.file_off, (int)rest, (int)nwrite);
            if (nwrite == -1) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN) {
                    return 0;
                } else {
                    LOG_WARN("sendfile: %s", strerror(errno));
                    return -1;
                }
            }

            if (chain->offset.file_off == chain->data.sendfile.size) {
                tmp = chain;
                chain = chain->next;
                free(tmp);
                LOG_DEBUG("sendfile end");
                goto sendloop;
            }

        }
    } else if (chain->type == MEMORY_CHAIN) {
        int cnt;
        struct iovec iovec[64];
        for (;;) {

          cnt = 0;
          tmp = chain;
          while (tmp && tmp->type == MEMORY_CHAIN) {
              iovec[cnt].iov_base = tmp->data.mem.base;
              iovec[cnt].iov_len = tmp->data.mem.len;
              ++cnt;
              tmp = tmp->next;
          }

          if (cnt == 0) {
              LOG_DEBUG("writev end");
              goto sendloop;
          }

          ssize_t nwrite;
          nwrite = writev(sockfd, iovec, cnt);
          LOG_DEBUG("writev %dbytes", nwrite);
          if (nwrite == -1) {
              if (errno == EINTR) {
                  continue;
              } else if (errno == EAGAIN) {
                  return 0;
              } else {
                  LOG_WARN("write: %s", strerror(errno));
                  return -1;
              }
          }

          while ((uint64_t)nwrite >= chain->data.mem.len) {
              tmp = chain;
              chain = chain->next;
              free(tmp);
          }

        }
    }

    return 0;
}

