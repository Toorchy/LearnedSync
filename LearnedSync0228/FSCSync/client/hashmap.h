// Copyright 2020 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define panic(_msg_)                                                         \
    {                                                                        \
        fprintf(stderr, "panic: %s (%s:%d)\n", (_msg_), __FILE__, __LINE__); \
        exit(1);                                                             \
    }

struct hashmap
{
    void *(*malloc)(size_t);
    void *(*realloc)(void *, size_t);
    void (*free)(void *);
    bool oom;
    size_t elsize;
    size_t cap;
    uint64_t seed0;
    uint64_t seed1;
    uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1);
    int (*compare)(const void *a, const void *b, void *udata);
    void (*elfree)(void *item);
    void *udata;
    size_t bucketsz;
    size_t nbuckets;
    size_t count;
    size_t mask;
    size_t growat;
    size_t shrinkat;
    void *buckets;
    void *spare;
    void *edata;
};

struct bucket
{
    uint64_t hash : 48;
    uint64_t dib : 16;
};

static struct bucket *bucket_at(struct hashmap *map, size_t index)
{
    return (struct bucket *)(((char *)map->buckets) + (map->bucketsz * index));
}

static void *bucket_item(struct bucket *entry)
{
    return ((char *)entry) + sizeof(struct bucket);
}

static void *(*_malloc)(size_t) = NULL;
static void *(*_realloc)(void *, size_t) = NULL;
static void (*_free)(void *) = NULL;

struct hashmap *hashmap_new_with_allocator(
    void *(*malloc)(size_t),
    void *(*realloc)(void *, size_t),
    void (*free)(void *),
    size_t elsize, size_t cap,
    uint64_t seed0, uint64_t seed1,
    uint64_t (*hash)(const void *item,
                     uint64_t seed0, uint64_t seed1),
    int (*compare)(const void *a, const void *b,
                   void *udata),
    void (*elfree)(void *item),
    void *udata)
{
    _malloc = /*_malloc ? _malloc : */ malloc;
    _realloc = /*_realloc ? _realloc : */ realloc;
    _free = /*_free ? _free : */ free;
    int ncap = 16;
    if (cap < ncap)
        cap = ncap;
    else
    {
        while (ncap < cap)
            ncap *= 2;
        cap = ncap;
    }
    size_t bucketsz = sizeof(struct bucket) + elsize;
    while (bucketsz & (sizeof(uintptr_t) - 1))
        bucketsz++;
    // hashmap + spare + edata
    size_t size = sizeof(struct hashmap) + bucketsz * 2;
    struct hashmap *map = _malloc(size);
    if (!map)
    {
        return NULL;
    }
    memset(map, 0, sizeof(struct hashmap));
    map->elsize = elsize;
    map->bucketsz = bucketsz;
    map->seed0 = seed0;
    map->seed1 = seed1;
    map->hash = hash;
    map->compare = compare;
    map->elfree = elfree;
    map->udata = udata;
    map->spare = ((char *)map) + sizeof(struct hashmap);
    map->edata = (char *)map->spare + bucketsz;
    map->cap = cap;
    map->nbuckets = cap;
    map->mask = map->nbuckets - 1;
    map->buckets = _malloc(map->bucketsz * map->nbuckets);
    if (!map->buckets)
    {
        _free(map);
        return NULL;
    }
    memset(map->buckets, 0, map->bucketsz * map->nbuckets);
    map->growat = map->nbuckets * 0.75;
    map->shrinkat = map->nbuckets * 0.10;
    map->malloc = _malloc;
    map->realloc = _realloc;
    map->free = _free;
    return map;
}

void *hashmap_get(struct hashmap *map, const void *key)
{
    if (!key)
    {
        panic("key is null");
    }
    uint64_t hash = map->hash(key, map->seed0, map->seed1) << 16 >> 16;
    size_t i = hash & map->mask;
    for (;;)
    {
        struct bucket *bucket = bucket_at(map, i);
        if (!bucket->dib)
            return NULL;
        if (bucket->hash == hash &&
            map->compare(key, bucket_item(bucket), map->udata) == 0)
            return bucket_item(bucket);
        i = (i + 1) & map->mask;
    }
}

static bool resize(struct hashmap *map, size_t new_cap)
{

    struct hashmap *map2 = hashmap_new_with_allocator(
        (_malloc ? _malloc : malloc),
        (_realloc ? _realloc : realloc),
        (_free ? _free : free),
        map->elsize, new_cap, map->seed1, map->seed1, map->hash, map->compare, map->elfree, map->udata);

    if (!map2)
        return false;
    for (size_t i = 0; i < map->nbuckets; i++)
    {
        struct bucket *entry = bucket_at(map, i);
        if (!entry->dib)
            continue;
        entry->dib = 1;
        size_t j = entry->hash & map2->mask;
        for (;;)
        {
            struct bucket *bucket = bucket_at(map2, j);
            if (bucket->dib == 0)
            {
                memcpy(bucket, entry, map->bucketsz);
                break;
            }
            if (bucket->dib < entry->dib)
            {
                memcpy(map2->spare, bucket, map->bucketsz);
                memcpy(bucket, entry, map->bucketsz);
                memcpy(entry, map2->spare, map->bucketsz);
            }
            j = (j + 1) & map2->mask;
            entry->dib += 1;
        }
    }
    map->free(map->buckets);
    map->buckets = map2->buckets;
    map->nbuckets = map2->nbuckets;
    map->mask = map2->mask;
    map->growat = map2->growat;
    map->shrinkat = map2->shrinkat;
    map->free(map2);
    return true;
}

void *hashmap_set(struct hashmap *map, void *item)
{
    if (!item)
        panic("item is null");
    map->oom = false;
    if (map->count == map->growat)
        if (!resize(map, map->nbuckets * 2))
        {
            map->oom = true;
            return NULL;
        }

    struct bucket *entry = map->edata;
    entry->hash = map->hash(item, map->seed0, map->seed1) << 16 >> 16;
    entry->dib = 1;
    memcpy(bucket_item(entry), item, map->elsize);

    size_t i = entry->hash & map->mask;
    for (;;)
    {
        struct bucket *bucket = bucket_at(map, i);
        if (bucket->dib == 0)
        {
            memcpy(bucket, entry, map->bucketsz);
            map->count++;
            return NULL;
        }
        if (entry->hash == bucket->hash &&
            map->compare(bucket_item(entry), bucket_item(bucket),
                         map->udata) == 0)
        {
            // memcpy(map->spare, bucket_item(bucket), map->elsize);
            // memcpy(bucket_item(bucket), bucket_item(entry), map->elsize);
            memcpy(map->spare, bucket_item(entry), map->elsize);
            return map->spare;
        }
        if (bucket->dib < entry->dib)
        {
            memcpy(map->spare, bucket, map->bucketsz);
            memcpy(bucket, entry, map->bucketsz);
            memcpy(entry, map->spare, map->bucketsz);
        }
        i = (i + 1) & map->mask;
        entry->dib += 1;
    }
}

// DEPRECATED: use `hashmap_new_with_allocator`
#endif
