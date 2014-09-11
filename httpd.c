#include <stdlib.h>

#include "server.h"

int main(int argc, char** argv)
{
    struct server* svc = new_http_server(80);
    serve(svc);

    exit(-1);
}
