#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "jhash.h"
#include "hashmap.h"
#ifdef DEBUG
#include <stdio.h>
#endif

struct hashmap* hashmap_init(
        int positions,
        int (*cmp)(const char* key1, const char* key2), 
        void (*del)(void* data))
{
    struct hashmap* map = NULL;
    map = (struct hashmap*)malloc(sizeof(struct hashmap));
    map->table = (void**)malloc(positions*sizeof(struct table_entry*));
    map->compare=cmp;
    map->destroy=del;
    map->size=0;
    map->numpos=positions;
#ifdef DEBUG
    printf("map->numpos: %u\n",map->numpos);
#endif
    for(int i=0; i<positions; ++i)
        map->table[i]=NULL;
    return map;
}

void hashmap_destroy(struct hashmap* map)
{
    if(NULL == map)
        return;
    for(int i=0;i<map->numpos;++i)
    {
        if(NULL == map->table[i])
            continue;
        free((map->table[i])->key);
        map->destroy((map->table[i])->data);
        free(map->table[i]);
    }
    free(map->table);
    memset(map,0,sizeof(struct hashmap));
    return;
}

uint32_t calculate_hash(int max, const char* key, int i)
{
    if(NULL == key)
        return 0;
    uint32_t key1 = i, key2 = (i*i);
    hashlittle2(key,strlen(key),&key1,&key2);
#ifdef DEBUG
    printf("map->numpos: %u\n",max);
#endif
#ifdef DEBUG
    uint32_t hashvalue = 0;
    hashvalue = key1 + (key2 * (i*key2));
    printf("before modding: hashvalue=%u\n",hashvalue);
    hashvalue = hashvalue % max;
    printf("after hashvalue % map->numpos(%u): %u\n",max,hashvalue);
    return hashvalue;
#else
    return ((key1 + (key2 * (i*key2)))%max);
#endif
}

int hashmap_insert(struct hashmap* map, const char* key, const void* data)
{
    if(NULL == map || NULL == key)
        return -1;
    for(int i=1; i<map->numpos; ++i)
    {
        uint32_t hashvalue;
        if(0 == (hashvalue = calculate_hash(map->numpos,key,i)))
            break;
        if(NULL == map->table[hashvalue] ||
                0 == map->compare((map->table[hashvalue])->key, key))
        {
            (map->table[hashvalue])=(struct table_entry*)malloc(sizeof(struct table_entry));
            (map->table[hashvalue])->key = malloc(strlen(key)+1);
            strcpy((map->table[hashvalue])->key,key);
            (map->table[hashvalue])->data = data;
            return 0;
        }
    }
    return -1;
}

int hashmap_remove(struct hashmap* map, const char* key)
{
    if(NULL == map || NULL == key)
        return -1;
    for(int i=1;i<map->numpos;++i)
    {
        uint32_t hashvalue;
        if(0 == (hashvalue = calculate_hash(map->numpos,key,i)))
            break;
        if(NULL == (map->table[hashvalue]))
            continue;
        if(0 == map->compare((map->table[hashvalue])->key,key))
        {
            free((map->table[hashvalue])->key);
            map->destroy((map->table[hashvalue])->data);
            free(map->table[hashvalue]);
            map->table[hashvalue]=NULL;
            return 0;
        }
    }
    return -1;
}

void* hashmap_lookup(const struct hashmap* map, const char* key)
{
    if(NULL == map || NULL == key)
        return NULL;
    for(int i=1;i<map->numpos;++i)
    {
        uint32_t hashvalue;
        if(0 == (hashvalue = calculate_hash(map->numpos,key,i)))
            break;
        if(NULL == map->table[hashvalue])
            continue;
        if(0 == (map->compare((map->table[hashvalue]->key),key)))
        {
            return ((map->table[hashvalue])->data);
        }
    }
    return NULL;
}

#ifdef DEBUG
void print_keys(const struct hashmap* map)
{
    if(NULL == map)
        return;
    for(int i=0;i<map->numpos;++i)
    {
        if(NULL != map->table[i])
            if(NULL != map->table[i]->key)
                    printf("%s[%u]\n",map->table[i]->key,i);
    }
}
#endif
