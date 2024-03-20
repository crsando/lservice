#include "util.h"

#include <unistd.h>
#include <stddef.h>

void util_usleep(unsigned int u) {
    usleep((useconds_t)u);
}