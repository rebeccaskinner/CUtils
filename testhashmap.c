#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "hashmap.h"

void destroy(void* data)
{
    free(data);
}

int compare(const char* key1, const char* key2)
{
    return strcmp(key1,key2);
}

int main(int argc UNUSED, char** argv UNUSED)
{
    struct hashmap* m;
    m = hashmap_init(30,compare,destroy);
    char* a = malloc(1024*sizeof(char));
    char* b = malloc(1024*sizeof(char));
    char* c = malloc(1024*sizeof(char));
    char* d = malloc(1024*sizeof(char));
    char* e;

    strcpy(a,"hello world");
    strcpy(b,"foo bar");
    strcpy(c,"boing boing");
    strcpy(d,"test test test");

    hashmap_insert(m,"hello",a);
    hashmap_insert(m,"foo",b);
    hashmap_insert(m,"boing",c);
    hashmap_insert(m,"test",d);

    e = hashmap_lookup(m, "hello");
    printf("hello=>%s\n",e);

    e = hashmap_lookup(m, "foo");
    printf("foo=>%s\n",e);

    e = hashmap_lookup(m, "boing");
    printf("boing=>%s\n",e);

    e = hashmap_lookup(m, "test");
    printf("test=>%s\n",e);

    e = hashmap_lookup(m, "hello");
    printf("hello=>%s\n",e);

#ifdef DEBUG
    print_keys(m);
    print_keys(m);
#endif

    e = hashmap_lookup(m, "hello");
    printf("test=>%s\n",e);

    e = hashmap_lookup(m, "hello");
    printf("test=>%s\n",e);

    hashmap_remove(m,"hello");

    if(NULL == (e = hashmap_lookup(m,"hello")))
    {
        printf("remove succeeded!\n");
        a = malloc(1024*sizeof(char));
        strcpy(a,"hello world");
    }

    hashmap_insert(m,"hello",a);
    e = hashmap_lookup(m, "hello");
    printf("hello=>%s\n",e);

    if(-1 == hashmap_insert(m,"hello",a))
        printf("overwrite failed...success!\n");

    e = hashmap_lookup(m, "hello");
    printf("test=>%s\n",e);

    hashmap_destroy(m);
    free(m);
    return 0;
}
