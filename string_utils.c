#include <string.h>
#include "string_utils.h"


u_char*
str_substr(u_char *start, u_char *end,  const char *substr)
{
    size_t i, len = strlen(substr);
    u_char *p;

    while (start + len - 1 != end) {
        i = 0;
        p = start;
        while (i != len && *p == substr[i]) {
            ++i;
            ++p;
        }

        if (i == len) {
            return start;
        } else {
            ++start;
        }
    }
    
    return NULL;
}
