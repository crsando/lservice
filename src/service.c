#include "service.h"
#include "cond.h"

#include <stdlib.h>

#define _SERVICE_MQ_DEF_SIZE_ 1024

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
    service_t * s = (service_t *)arg;

    s->init(s, NULL);

    // run once
    while (1) {
        cond_wait_begin(s->c);
        cond_wait(s->c);
        void * msg = queue_pop_ptr(s->q);
        cond_wait_end(s->c);

        s->routine(s, msg);
    }
}

int service_start(service_t * s, service_routine_t routine, service_routine_t init, void * init_params) {
    pthread_t th;

    s->routine = routine;
    s->init = init;
    s->init_params = init_params;
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