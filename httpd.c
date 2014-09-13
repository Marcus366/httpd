#include <stdlib.h>

#include "http_srv.h"

int main(int argc, char** argv)
{
    struct http_srv* srv = new_http_srv(80);
    if (srv != NULL) {
        serve(srv);
    }
    //never return
    exit(-1);
}
