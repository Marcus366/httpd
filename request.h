#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <unistd.h>

enum req_state {
    REQ_INVAL,
    REQ_PARSE_BEGIN,
    REQ_CL,
    REQ_CLRF,
    REQ_CLRFCL,
    REQ_PARSE_END
};

struct request {
    enum req_state state;
    char* read_buf;
    size_t buf_size, read_idx, check_idx;
    
};

#endif
