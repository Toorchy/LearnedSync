#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>

typedef struct chunk
{
    int id;
    int offset;
    int length;
    uint64_t weakhash;
} chunk;

typedef struct chunkList
{
    int size;
    int capcity;
    chunk *chunks;
} chunkList;

typedef struct FileInfo
{
    // 设定目录长度小于256，文件名长度小于128
    char filename[128];
    char dirname[256];
    off_t size;
    struct timespec lastmodifytime;
} FileInfo;

int write_int(int fd, int num)
{
    uint32_t net_num = htonl(num);
    return write(fd, &net_num, sizeof(uint32_t));
}

int read_int(int fd)
{
    uint32_t net_num = 0;
    recv(fd, (char *)&net_num, sizeof(uint32_t), MSG_WAITALL);
    return ntohl(net_num);
}
