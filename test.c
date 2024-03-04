#include "src/queue.h"
#include "src/service.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void * init(void * s, void * msg) {
    printf("job init\n");
    return NULL;
}

void * job(void * s, void * msg) {
    printf("job begins \n");
    // do something time consuming
    sleep(10);
    printf("job ends");
    return NULL;
}

const char msg[32] = "Hello";


int main(int argc, char ** argv) {
    // struct queue * q;
    // int i;
    // q = queue_new_int(32);

    // for(i=1;i<32;i++) {
    //     queue_push_int(q, i*i);
    // }

    // while( queue_length(q) > 0) {
    //     i = queue_pop_int(q);
    //     printf("pop: %d\n",i);
    // }

    // printf("Hello World\n");


    service_t * s;
    s = service_new();

    service_start(s, job, init, NULL);

    while(1) {
        int n = queue_length(s->q);
        printf("service_wake s | %d\n", n);
        service_send(s, (void*)msg);
        sleep(1);
    }

    return 0;
}
