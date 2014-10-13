#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_config.h"
#include "http_log.h"


void
http_reigster_config_directive(const char *name, config_handler handler) {
    http_config_directive *d = (http_config_directive*)malloc(sizeof(http_config_directive));

    d->name    = strdup(name);
    d->handler = handler;
    list_add_after(&d->list, &directive->list);
}

http_config*
http_create_config()
{
    listnode *list = &directive->list.next;
    http_config *conf = (http_config*)malloc(sizeof(http_config));

    conf->L = luaL_newstate();
    luaL_openlibs(conf->L);

    while (list != &directive->list) {
        http_config_directive *d = container_of(list, http_config_directive, list);
        lua_register(conf->L, d->name, d->handler);
        list = list->next;
    } 

    return conf;
}
