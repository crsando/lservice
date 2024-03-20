#include <pthread.h>
#include "queue.h"
#include "cond.h"
#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "registry.h"

typedef unsigned long long service_id;

typedef struct {
    pthread_mutex_t lock;
    registry_t * services;
    registry_t * variables;
} service_pool_t;

typedef struct {
    service_pool_t * pool;
    char name[32];

    // init params
    char * code;
    void * config;
    // service_id id;

    // multi-thread utilities
    pthread_t thread;
    struct queue * q;
    struct cond * c;


    lua_State * L;
    int lua_func_ref;
} service_t;

service_pool_t * service_pool_new();
void * service_pool_registry(service_pool_t * pool, const char * key, void * ptr);
service_t * service_pool_query_service(service_pool_t * pool, const char * key);

// service_t * service_new(service_pool_t * pool, const char * name);
service_t * service_new(service_pool_t * pool, const char * name, const char * code, void * config);

// int service_init_lua(service_t * s, const char * code, void * config);
int service_init_lua(service_t * s);
int service_routine_lua(service_t * s, void * msg);
int service_start(service_t * s);

int service_send(service_t * s, void * msg);
int service_free(service_t * s);

// typedef struct {
//     uint32_t type;
//     void * body;
//     size_t size;
// } message_t;