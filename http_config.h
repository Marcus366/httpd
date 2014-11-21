#ifndef __HTTP_CONF_H__
#define __HTTP_CONF_H__


#include <lua.h>
#include <lauxlib.h>
#include "http_log.h"
#include "container/list.h"


#define CONFIG_INTERFACE(ret_val, func_name) \
    static ret_val func_name(lua_State *L)
#define CONFIG_VARIABLE(type, name) \
    static type name


typedef lua_CFunction config_handler;


typedef struct http_config_directive {
    const char     *name;
    config_handler  handler;
    listnode        list;
} http_config_directive;


typedef struct http_config {
    lua_State               *L;
    listnode                directives;
} http_config;



http_config* http_create_config();
void         http_free_config(http_config *config);


void http_reigster_config_directive(http_config *config, const char *name, config_handler handler);


void http_load_config(http_config *config, const char *filename);
void http_reload_config(http_config *config, const char *filename);


#endif
