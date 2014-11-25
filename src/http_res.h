#ifndef __HTTP_RES_H__
#define __HTTP_RES_H__

#include "http_request.h"

//HTTP Status Code
#define HTTP_OK                      "200 OK"
#define HTTP_CREATED                 "201"
#define HTTP_ACCEPTED                "202"
#define HTTP_NO_CONTENT              "204"
#define HTTP_MOVED_PERMANENTLY       "301"
#define HTTP_MOVED_TEMPORARILY       "302"
#define HTTP_NOT_MODIFIED            "303"
#define HTTP_BAD_REQUEST             "400"
#define HTTP_UNAUTHORIZED            "401"
#define HTTP_FORBIDDEN               "403"
#define HTTP_NOT_FOUND               "404"
#define HTTP_INTERNAL_SERVER_ERROR   "500"
#define HTTP_NOT_IMPLEMENTED         "501"
#define HTTP_BAD_GATEWAY             "502"
#define HTTP_SERVICE_UNAVAILABLE     "503"

enum res_state {
    REQ_GEN_BEGIN,
    REQ_GEN_END,
    REQ_SEND_HEADER,
    REQ_SEND_BODY,
    REQ_SEND_END
};

enum send_state {
    SEND_ERROR,
    SEND_BLOCK,
    SEND_FINISH
};

struct http_res {
    enum res_state  state;
    char           *send_buf;
    size_t          buf_size;
    size_t          buf_len;
    size_t          send_idx;


    struct http_fcache_file *file;
    off_t                    offset;
};

struct http_res* new_http_res();
void             free_http_res(struct http_res *res);

/*
 * Generate Response.
 * It is called after parse the request, and generate the response
 * string in the buffer.
 */
int http_gen_res(struct http_res *res, http_request_t* req);

/*
 * Send the response to sockfd.
 * It will be blocked if the TCP send buffer is full,
 * so it return the send state for more operation.
 */
enum send_state http_send_res(struct http_res *res, int sockfd);

#endif