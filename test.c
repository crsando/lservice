#include "src/queue.h"
#include "src/service.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "string.h"

char * read_file(const char * fn, char * buffer) {
    FILE * f;
    char ch;
    char * p;

    f = fopen(fn, "r");
    p = buffer;
    while ( (ch = fgetc(f)) != EOF ) {
        *p++ = ch;
    }
    return buffer;
}

const char msg[32] = "Hello";

int main(int argc, char ** argv) {
    char buffer[4096];
    service_t * s;
    s = service_new();

    memset(buffer, 0, sizeof(buffer));
    read_file("service/hello.lua", buffer);
    service_init_lua(s, buffer);
    service_start(s);

    while(1) {
        int n = queue_length(s->q);
        printf("service_wake s | %d\n", n);
        service_send(s, (void*)msg);
        sleep(1);
    }

    return 0;
}
