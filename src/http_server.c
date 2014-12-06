#include <stdlib.h>
#include <string.h>
#include "http_server.h"


http_server_t*
http_create_server(const char *root)
{
    http_server_t *svc = (http_server_t*)malloc(sizeof(http_server_t));

    if (svc == NULL) {
        return NULL;
    }

    svc->host = strdup("*");
    svc->root = strdup(root);
    svc->next = NULL;

    return svc;
}


http_server_t*
http_find_server(http_server_t *head, const char *host)
{
    (void) host;
    return head;
}

