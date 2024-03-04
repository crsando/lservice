#include "src/queue.h"
#include "src/routine.h"
#include "src/service.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

const char code[32] = "print(";


int main(int argc, char ** argv) {
    run_lua_code(code);
    return 0;
}