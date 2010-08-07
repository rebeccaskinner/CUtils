#ifndef __hashmap_h__
#define __hashmap_h__
#include <stdint.h>

#define hashmap_size(hashmap) ((hashmap)->size)

/*typedef */struct table_entry
{
    char* key;
    void* data;
};

/*typedef */struct hashmap
{
    uint32_t size;
    uint32_t numpos;
    struct table_entry** table;
    void (*destroy)(void* data);
    int (*compare)(const char* key1, const char* key2);
};

struct hashmap* hashmap_init(
        int positions,
        int (*cmp)(const char* key1, const char* key2), 
        void (*del)(void* data));

void hashmap_destroy(struct hashmap* map);
uint32_t calculate_hash(int max, const char* key, int i);
int hashmap_insert(struct hashmap* map, const char* key, const void* data);
int hashmap_remove(struct hashmap* map, const char* key);
void* hashmap_lookup(const struct hashmap* map, const char* key);
#ifdef DEBUG
void print_keys(const struct hashmap* map);
#endif
#endif
