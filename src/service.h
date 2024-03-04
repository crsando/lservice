#include <pthread.h>
#include "queue.h"
#include "cond.h"

typedef unsigned long long service_id;

typedef void * (*service_routine_t)(void * s, void * msg);

typedef struct {
    service_id id;
    struct queue * q;


    pthread_t thread;
    service_routine_t init;
    service_routine_t routine;
    void * init_params;

    struct cond * c;
} service_t;

service_t * service_new();
int service_start(service_t * s, service_routine_t routine, service_routine_t init, void * init_params);
int service_send(service_t * s, void * msg);

typedef struct {
    uint32_t type;
    void * body;
    size_t size;
} message_t;