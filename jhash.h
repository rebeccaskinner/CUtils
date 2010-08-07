#ifndef __jhash_h__
#define __jhash_h__
/*
 * this header file was hacked up from jhash.c which is based on Bob Jenkins'
 * lookup3.c, found in jhash.c
 * lookup3.c, by Bob Jenkins, May 2006, Public Domain.
*/
#include <stdio.h>      /* defines printf for tests */
#include <time.h>       /* defines time_t for timings in the test */
#include <stdint.h>     /* defines uint32_t etc */
#include <sys/param.h>  /* attempt to define endianness */
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
        __BYTE_ORDER == __LITTLE_ENDIAN) || \
(defined(i386) || defined(__i386__) || defined(__i486__) || \
 defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL))
# define HASH_LITTLE_ENDIAN 1
# define HASH_BIG_ENDIAN 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
        __BYTE_ORDER == __BIG_ENDIAN) || \
(defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 1
#else
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 0
#endif

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
    a -= c;  a ^= rot(c, 4);  c += b; \
    b -= a;  b ^= rot(a, 6);  a += c; \
    c -= b;  c ^= rot(b, 8);  b += a; \
    a -= c;  a ^= rot(c,16);  c += b; \
    b -= a;  b ^= rot(a,19);  a += c; \
    c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
}

uint32_t hashword(const uint32_t *k, size_t length, uint32_t initval);
void hashword2 (const uint32_t *k, size_t length, uint32_t *pc, uint32_t *pb);
uint32_t hashlittle(const void *key, size_t length, uint32_t initval);
void hashlittle2(const void *key, size_t length, uint32_t *pc,uint32_t *pb);
uint32_t hashbig(const void *key, size_t length, uint32_t initval);
#endif
