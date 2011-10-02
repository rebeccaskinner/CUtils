#include "utils.h"
#include <stdlib.h>
#include <string.h>

void* malloc0(size_t s)
{
    void* x = malloc(s);
    memset(x,0,s);
    return x;
}

void free0(void** m)
{
    if(UNLIKELY(!m || !*m)) return;
    free(*m);
    *m = NULL;
}
