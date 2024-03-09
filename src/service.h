#include <pthread.h>
#include "queue.h"
#include "cond.h"
#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

typedef unsigned long long service_id;


typedef struct {
    service_id id;
    struct queue * q;

    pthread_t thread;
    struct cond * c;

    lua_State * L;
    int lua_func_ref;
} service_t;

service_t * service_new();

int service_init_lua(service_t * s, const char * code);
int service_routine_lua(service_t * s, void * msg);
int service_start(service_t * s);
int service_send(service_t * s, void * msg);
int service_free(service_t * s);

// typedef struct {
//     uint32_t type;
//     void * body;
//     size_t size;
// } message_t;