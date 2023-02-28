#include "chunkList.h"
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "match.h"
#include "fastcdc.h"

#define PORT 7796
#define CacheSize 1024 * 1024 * 1024

struct timeval chunking_start, chunking_end;

int main(int argc, char const *argv[])
{
    gettimeofday(&startTime, NULL);

    if (argc < 4)
    {
        printf("Usage:%s<IP><DIR><FILE>\n", argv[0]);
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {
        AF_INET,
        htons(PORT),
        *((struct in_addr *)gethostbyname(argv[1])->h_addr)};
    connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr));

    /* access files */
    char path[1000];
    strcpy(path, argv[2]);
    strcat(path, argv[3]);
    struct stat st;
    stat(path, &st);
    FileInfo info = {{0}, {0}, st.st_size, st.st_mtim};
    strcpy(info.dirname, argv[2]);
    strcpy(info.filename, argv[3]);

    /* send file infomation and receive flag */
    send_volume += write(sockfd, &info, sizeof(FileInfo));
    FILE *fp = fopen(path, "r");

    if (read_int(sockfd))
    {
        printf("=== delta sync ===\n");

        /* chunking and receiving chunk list */
        gettimeofday(&chunking_start, NULL);
        unsigned char *fileCache = (char *)malloc(CacheSize);
        chunkList list = cdchunking(fp, fileCache), farlist;
        gettimeofday(&chunking_end, NULL);
        printf("Chunking completed.\n");

        farlist.size = read_int(sockfd);
        int readsize = farlist.size * sizeof(chunk);
        farlist.chunks = malloc(readsize);
        printf("readsize:%d, farlist.size:%d\n", readsize, farlist.size);
        recv_volume += recv(sockfd, farlist.chunks, readsize, MSG_WAITALL) + 4;
        printf("success recv chunk list\n");

        /* match,send delta data to server */
        send_volume += write_int(sockfd, list.size);
        match(list, farlist, fileCache, sockfd);
    }
    else
    {
        printf("=== full sync ===\n");
        char tempbuf[32768];
        for (int readsize = 0; (readsize = fread(tempbuf, 1, 32768, fp)) > 0;)
            send_volume += write(sockfd, tempbuf, readsize);
    }

    /* recv ack */
    read_int(sockfd);

    fclose(fp);

    close(sockfd);

    gettimeofday(&endTime, NULL);

    float totalTime = ((endTime.tv_sec - startTime.tv_sec) * 1000000 +
                       (endTime.tv_usec - startTime.tv_usec)),
          chunkingTime = ((chunking_end.tv_sec - chunking_start.tv_sec) * 1000000 +
                          (chunking_end.tv_usec - chunking_start.tv_usec));

    printf("totalTime is %f s\n", totalTime / 1000000);
    printf("chunkingTime is %f s\n", chunkingTime / 1000000);
    printf("total send %d byte\n", send_volume);
    printf("total recv %d byte\n", recv_volume + 8);
    return 0;
}
