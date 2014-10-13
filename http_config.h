#ifndef __HTTP_CONF_H__
#define __HTTP_CONF_H__


#include <lua.h>
#include <luaxlib.h>
#include "http_log.h"
#include "container/list.h"


#define CONFIG_INTERFACE(ret_val, func_name) \
    static ret_val func_name(lua_State *L)


typedef lua_CFunction config_handler;


typedef struct http_config_directive {
    const char     *name;
    config_handler  handler;
    struct lnode    list;
} http_config_directive;

http_config_directive directive;


struct http_config {
    lua_State *L;
} http_config;


void http_register_config_directive(const char *name, config_handler handler);


http_config* http_create_config();

#endif
