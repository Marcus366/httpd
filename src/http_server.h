#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__


#include <sys/time.h>


typedef struct http_server http_server_t;
struct http_server {
    char          *host;
    char          *root;

    http_server_t *next;
};


http_server_t* http_create_server(const char *root);


http_server_t* http_find_server(http_server_t *head, const char *host);

#endif

