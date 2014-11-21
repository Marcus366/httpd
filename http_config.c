#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_config.h"
#include "http_log.h"


void
http_reigster_config_directive(http_config *config, const char *name, config_handler handler) {
    http_config_directive *d = (http_config_directive*)malloc(sizeof(http_config_directive));

    d->name    = strdup(name);
    d->handler = handler;
    list_add_after(&d->list, &config->directives);
}


http_config*
http_create_config()
{
    http_config *conf = (http_config*)malloc(sizeof(http_config));

    conf->L = luaL_newstate();
    list_init(&conf->directives);
    //luaL_openlibs(conf->L);

    return conf;
}


void
http_free_config(http_config *config)
{
    listnode *node;
    http_config_directive *d;

    node = config->directives.next;
    while (node != &config->directives) {
        d = (http_config_directive*)container_of(node, http_config_directive, list);
        node = node->next;

        free(d->name);
        free(d);
    }

    lua_close(config->L);
    free(config);
}

void
http_load_config(http_config *config, const char *filename)
{
    int err;
    listnode *list = config->directives.next;

    while (list != &config->directives) {
        http_config_directive *d =
            (http_config_directive*)container_of(list, http_config_directive, list);
        lua_register(config->L, d->name, d->handler);
        list = list->next;
    } 

    err = luaL_dofile(config->L, filename);
    if (err) {
        LOG_ERROR("parse config file failed");
        exit(EXIT_FAILURE);
    }

}

void
http_reload_config(http_config *config, const char *filename)
{
    int err = luaL_dofile(config->L, filename);
    if (err) {
        LOG_ERROR("parse config file failed");
        exit(EXIT_FAILURE);
    }
}

