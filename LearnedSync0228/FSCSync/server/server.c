#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <syslog.h>
#include <arpa/inet.h>
#include "chunkList.h"
#include "fastcdc.h"

#define PORT 7796
#define BACKLOG 5
#define CacheSize 1024 * 1024 * 1024
#define ID 'c'

char *storepath = "/home/zhouyu/store1/";

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    char filepath[128], newpath[128];
    struct sockaddr_in server = {AF_INET, htons(PORT), htonl(INADDR_ANY)};

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        syslog(LOG_NOTICE | LOG_LOCAL0, "socket error.");
        exit(-1);
    }

    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        syslog(LOG_NOTICE | LOG_LOCAL0, "bind error.");
        exit(-1);
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        syslog(LOG_NOTICE | LOG_LOCAL0, "listen error.");
        exit(-1);
    }

    FileInfo *info = malloc(sizeof(FileInfo));

    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);

        // recv_info(info, connfd);
        read(connfd, info, sizeof(FileInfo));

        FILE *fileptr;
        strcpy(filepath, storepath);
        strcat(filepath, info->filename);

        char tempbuf[32768];

        /* Whether the old version file exists */
        if (fileptr = fopen(filepath, "r"))
        {
            write_int(connfd, true);

            /* chunking */
            unsigned char *filecache = (char *)malloc(CacheSize);
            chunkList list = cdchunking(fileptr, filecache);

            /* send chunking message */
            write_int(connfd, list.size);
            write(connfd, list.chunks, list.size * sizeof(chunk));

            /* use temp file */
            strcpy(newpath, filepath);
            strcat(newpath, ".tmp");
            FILE *tempfile = fopen(newpath, "w");

            /* build new File */
            int ptr = 0, length, chunkID, offset, totalchunknum = read_int(connfd);
            for (int chunknum = 0; chunknum != totalchunknum; chunknum++)
            {
                /* Whether it is a redundant block */
                if (read_int(connfd))
                {
                    // read token id
                    chunkID = read_int(connfd);
                    length = list.chunks[chunkID].length;
                    offset = list.chunks[chunkID].offset;
                    ptr += length;
                    printf("id = %d, offset = %d, len = %d\n", chunkID, ptr, length);
                    fwrite(filecache + offset, length, 1, tempfile);
                }
                else
                {
                    // read data length
                    length = read_int(connfd);
                    ptr += length;
                    recv(connfd, tempbuf, length, MSG_WAITALL);
                    printf("%d  %d  %d\n", chunknum, ptr, length);
                    fwrite(tempbuf, length, 1, tempfile);
                }
            }
            // free cache
            free(filecache);

            /* Exchange of old and new files */
            rename(newpath, filepath);

            // close fileptr
            fclose(tempfile);
        }
        else
        {
            write_int(connfd, false);

            fileptr = fopen(filepath, "w");
            for (int readsize = 0, recv_volume = 0; recv_volume < info->size; recv_volume += readsize)
            {
                readsize = read(connfd, tempbuf, 32768);
                fwrite(tempbuf, readsize, 1, fileptr);
            }
        }

        // send ack
        write_int(connfd, true);

        close(connfd);
        fclose(fileptr);
    }

    close(listenfd);

    return 0;
}
