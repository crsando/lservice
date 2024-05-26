#include "service.h"

#include "log.h"
#include <assert.h>
#include <stdlib.h>

#define _SERVICE_MQ_DEF_SIZE_ 1024

service_pool_t * service_pool_new() {
    service_pool_t * pool = NULL;
    pool = (service_pool_t *)malloc(sizeof(service_pool_t));
    memset(pool, 0, sizeof(service_pool_t));
	pthread_mutex_init(&pool->lock, NULL);
    return pool;
}

service_t * service_pool_query_service(service_pool_t * pool, const char * key) {
    service_t * s = NULL;
    pthread_mutex_lock(&pool->lock);
    registry_t * r = registry_get(&pool->services, key);
    // log_debug("service_pool_query_service | registry_get %d | %s", r, r->key);
    s = (service_t*)( r ? r->ptr : NULL );
    pthread_mutex_unlock(&pool->lock);
    return s;
}

void * service_pool_registry(service_pool_t * pool, const char * key, void * ptr) {
    // log_debug("service_pool_registry : %d | %s | %d", pool, key, ptr);
    if(ptr) {
        pthread_mutex_lock(&pool->lock);
        registry_put(&pool->variables, key, ptr);
        pthread_mutex_unlock(&pool->lock);
        return ptr;
    }
    else {
        void * data = NULL;
        // get registry
        log_debug("lock pool->lock");
        pthread_mutex_lock(&pool->lock);
        if(! pool->variables ) {
            // log_debug("pool->variables == NULL");
            data = NULL;
        }
        else {
            // log_debug("pool->variables != NULL | registry_get | %d | %s", &pool->variables, key);
            registry_t * r = registry_get(&pool->variables, key);
            // if( r != NULL ) {
            //     log_debug("registry_get %d | %s | %d", r, r->key, r->ptr);
            // }
            // else {
            //     log_debug("registry_get NULL");
            // }
            data = ( r ? r->ptr : NULL );
        }
        pthread_mutex_unlock(&pool->lock);
        return data;
    }
}

// int service_init_lua(service_t * s, const char * code, void * config) {
int service_init_lua(service_t * s) {
    lua_State * L;
	L = luaL_newstate();
    if(!L) {
		log_error("THREAD FATAL ERROR: could not create lua state");
		return -1;
	}

	luaL_openlibs(L);

    if(luaL_loadstring(L, s->code)) {
        log_error("FATAL THREAD PANIC: (loadstring) %s", lua_tolstring(L, -1, NULL));
		lua_close(L);
		return -1; 
    }

    int n_args = 0;
    // push self
    // log_debug("s %d", s);
    lua_pushlightuserdata(L, s);
    n_args ++;

    // push lightuserdata (config)
    // log_debug("config: %d", s->config);
    if(s->config) {
        lua_pushlightuserdata(L, s->config);
        n_args ++;
    }


    // run the lua code, we expect the code will *return* a proper lua function
    // whenever some message arrives, this function will be executed
    // no config input, one output ( the routine function )
	if(lua_pcall(L, n_args, 1, 0)) {
		log_error("FATAL THREAD PANIC: (pcall) %s", lua_tolstring(L, -1, NULL));
		lua_close(L);
		return -1;
	}

    // save the function reference in the registry ( and pop the stack )
    s->lua_func_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    s->L = L;
    // s->service_type = 1; // lua service

    return 0; // no error
}

int service_routine_lua(service_t * s, void * msg) {
    lua_State * L = s->L;

    // retrieve function from lua registry
    lua_rawgeti(L, LUA_REGISTRYINDEX, s->lua_func_ref);
    lua_pushlightuserdata(L, msg);
    int errcode = lua_pcall(L, 1, 0, 0); // one input, no output
    if(errcode) {
        log_error("ERROR in lua routine : %s", lua_tolstring(L, -1, NULL));
        return -1;
    }
    return 0; // no error
}


service_t * service_new(service_pool_t * pool, const char * name, const char * code, void * config) {
    service_t * s;

    s = (service_t *)malloc(sizeof(service_t));
    memset(s, 0, sizeof(service_t));

    if(pool) {
        s->pool = pool;
        assert(name && (strlen(name) <= 30));
        strcpy(s->name, name);

        pthread_mutex_lock(&pool->lock);
        registry_put(&pool->services, name, s);
        pthread_mutex_unlock(&pool->lock);
    }

    assert(code != NULL);
    s->code = (char *)malloc(sizeof(char) * (strlen(code) + 1));
    strcpy(s->code, code);
    s->config = config;

    s->q = queue_new_ptr(_SERVICE_MQ_DEF_SIZE_);
    s->c = (struct cond *)malloc(sizeof(struct cond));
    cond_create(s->c);

    return s;
}

// entry fro pthread_create
void * service_routine_wrap(void * arg) {
    void * msg;
    service_t * s = (service_t *)arg;

    // init lua environment, load lua script
    service_init_lua(s);

    // assert(s->service_type == 1);
    assert(s->L != NULL);
    assert(s->lua_func_ref > 0);

    // s->init(s, NULL);

    // run once
    while (1) {
        cond_wait_begin(s->c);
        // log_debug("wait %s | %d", s->name, queue_length(s->q));

        if( queue_length(s->q) == 0 )
            cond_wait(s->c);
        // log_debug("wait step");

        // designated behaviour: throw msg away, leave only the last one
        // TODO: Fix memory
        // while( queue_length(s->q) > 0 )
        if( queue_length(s->q) > 0 )
            msg = queue_pop_ptr(s->q);

        cond_wait_end(s->c);

        // run lua code
        // log_debug("triggered %s", s->name);
        service_routine_lua(s, msg);
    }
}

int service_start(service_t * s) {
    pthread_t th;
    int ret = pthread_create(&th, NULL, service_routine_wrap, s);
    s->thread = th;
    return ret;
}

int service_send(service_t * s, void * msg) {
    cond_trigger_begin(s->c);
    queue_push_ptr(s->q, msg);
    cond_trigger_end(s->c, 1);
    return 1;
}

int service_free(service_t * s) {
    queue_delete(s->q);
    cond_release(s->c);
    lua_close(s->L);
    return 1;
}


// message_t * message_new(message_t *msg) {
// 	message_t * r = (message_t *)malloc(sizeof(*r));
// 	if (r == NULL)
// 		return NULL;
// 	*r = *msg;
// 	return r;
// }

// void message_delete(message_t *msg) {
// 	if (msg) {
// 		free(msg->body);
// 		free(msg);
// 	}
// }