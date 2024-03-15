#include "uthash.h"

typedef struct {
    char key[32];
    void * ptr;
    UT_hash_handle hh;
} registry_t;

registry_t * registry_get(registry_t ** reg, const char * key);
registry_t * registry_put(registry_t ** reg, const char * key, void * ptr);
registry_t * registry_del(registry_t ** reg, const char * key);