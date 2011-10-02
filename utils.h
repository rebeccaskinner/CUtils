#ifndef __UTILS_H__
#define __UTILS_H__
#include <sys/types.h>

#ifdef __GNUC__
    #define LIKELY(x) __builtin_expect((x),1l)
    #define UNLIKELY(x) __builtin_expect((x),0l)
    #define UNUSED __attribute__((unused))
#else
    #define LIKELY
    #define UNLIKELY
    #define UNUSED
#endif

void* malloc0(size_t) __attribute__((malloc));
void  free0(void**);
#endif
