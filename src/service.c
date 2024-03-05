#include "service.h"

#include "log.h"
#include <assert.h>
#include <stdlib.h>

#define _SERVICE_MQ_DEF_SIZE_ 1024


int service_init_lua(service_t * s, const char * code) {
    lua_State * L;
	L = luaL_newstate();
    if(!L) {
		log_error("THREAD FATAL ERROR: could not create lua state");
		return -1;
	}

	luaL_openlibs(L);

    if(luaL_loadstring(L, code)) {
        log_error("FATAL THREAD PANIC: (loadstring) %s", lua_tolstring(L, -1, NULL));
		lua_close(L);
		return -1; 
    }

    // run the lua code, we expect the code will *return* a proper lua function
    // whenever some message arrives, this function will be executed
    // no config input, one output ( the routine function )
	if(lua_pcall(L, 0, 1, 0)) {
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


service_t * service_new() {
    service_t * s;

    s = (service_t *)malloc(sizeof(service_t));
    s->q = queue_new_ptr(_SERVICE_MQ_DEF_SIZE_);

    s->c = (struct cond *)malloc(sizeof(struct cond));
    cond_create(s->c);

    return s;
}

// entry fro pthread_create
void * service_routine_wrap(void * arg) {
    void * msg;
    service_t * s = (service_t *)arg;

    // assert(s->service_type == 1);
    assert(s->L != NULL);
    assert(s->lua_func_ref > 0);

    // s->init(s, NULL);

    // run once
    while (1) {
        cond_wait_begin(s->c);
        cond_wait(s->c);

        // designated behaviour: throw msg away, leave only the last one
        while( queue_length(s->q) > 0 )
            msg = queue_pop_ptr(s->q);

        cond_wait_end(s->c);

        // run lua code
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