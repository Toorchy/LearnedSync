#include "hashmap.h"

int send_volume, recv_volume;

struct timeval startTime, endTime;

typedef struct hash2id
{
    uint64_t weakhash;
    int id;
} hash2id;

int compare_func(const void *a, const void *b, void *udata)
{
    const struct hash2id *ua = a, *ub = b;
    return ua->weakhash != ub->weakhash;
}

uint64_t hash_func(const void *item, uint64_t seed0, uint64_t seed1)
{
    const struct hash2id *hash = item;
    return hash->weakhash;
}

// getchunklist

int ismatch(uint64_t weakhash, struct hashmap *map)
{
    struct hash2id *hash = hashmap_get(map, &(hash2id){.weakhash = weakhash});
    if (hash)
        // printf("hash=%ld probably match in chunk %d\n", weakhash, hash->id);
        return hash->id;
    return -1;
}

void match(chunkList list, chunkList farlist, unsigned char *fileCache, int fd)
{
    /* Number of times strong and weak hashes need to be executed */
    int weak_match_time = 0, strong_match_time = 0;

    /* build hash table */
    struct hashmap *map = hashmap_new_with_allocator(
        (_malloc ? _malloc : malloc),
        (_realloc ? _realloc : realloc),
        (_free ? _free : free),
        sizeof(hash2id), 0, 0, 0, hash_func, compare_func, NULL, NULL);
    for (int i = 0; i < farlist.size; i++)
        hashmap_set(map, &(hash2id){.weakhash = farlist.chunks[i].weakhash, .id = farlist.chunks[i].id});

    // char match_flag;
    struct timeval matchStartTime, matchEndTime;
    gettimeofday(&matchStartTime, NULL);

    int id, length, offset, match_flag;
    for (int i = 0; i < list.size; i++)
    {
        id = ismatch(list.chunks[i].weakhash, map);
        weak_match_time++;

        if (id != -1)
        {
            // send token
            send_volume += write_int(fd, true);
            send_volume += write_int(fd, id);
            strong_match_time++;
        }
        else
        {
            // send data
            send_volume += write_int(fd, false);
            length = list.chunks[i].length;
            offset = list.chunks[i].offset;
            send_volume += write_int(fd, length);
            send_volume += write(fd, fileCache + offset, length);
        }
    }
    gettimeofday(&matchEndTime, NULL);
    float totalTm = ((matchEndTime.tv_sec - matchStartTime.tv_sec) * 1000000 +
                     (matchEndTime.tv_usec - matchStartTime.tv_usec));
    printf("total weak match %d\n", weak_match_time);
    printf("total strong match %d\n", strong_match_time);
    printf("match spend time %f s\n", totalTm / 1000000);
}
