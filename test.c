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


lua_State * L;

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

// int init_lua() {
//     char buffer[4096];
//     memset(buffer, 0, 4096*sizeof(char));
//     L = luaL_newstate();
//     luaL_openlibs(L);

//     read_file("tmp.lua", buffer);
//     printf("\n---\n");
//     printf(buffer);
//     printf("\n---\n");


//     if(luaL_loadstring(L, buffer)) {
//         printf("FATAL THREAD PANIC: (loadstring) %s\n", lua_tolstring(L, -1, NULL));
// 		lua_close(L);
// 		return -1; 
//     }
// 	if(lua_pcall(L, 0, 0, 0)) {
// 		printf("FATAL THREAD PANIC: (pcall) %s\n", lua_tolstring(L, -1, NULL));
// 		lua_close(L);
// 		return -1;
// 	}
//     return 0; // no error
// }

// void * init(void * s, void * msg) {
//     printf("job init\n");
//     return NULL;
// }

// void * job(void * s, void * msg) {
//     printf("job begins \n");
//     // do something time consuming
//     lua_getglobal(L, "test_run");
//     if(lua_pcall(L, 0, 0, 0)) {
//         printf("error running function `f': %s", lua_tostring(L, -1));
//     }

//     // 
//     sleep(10);
//     printf("job ends");
//     return NULL;
// }

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
