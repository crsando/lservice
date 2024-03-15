#include "src/registry.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    registry_t * reg = NULL;
    registry_t * p = NULL;
    char key1[32] = "hello";
    char v1[32] = "world";

    registry_put(&reg, &key1[0], &v1[0]);

    p = registry_get(&reg, &key1[0]);

    printf("get result: %s\n", (char*)(p->ptr));

    return 0;
}