#include "fileinfo.h"
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 7789
#define TEMPBUFSIZE 1024 * 1024

int main(int argc, char const *argv[])
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0), endflag;
    char tempbuf[TEMPBUFSIZE] = {0};
    struct sockaddr_in server = {AF_INET, htons(PORT), htonl(INADDR_ANY)};
    FILE *fp;
    FileInfo *info = malloc(sizeof(FileInfo));

    bind(listenfd, (struct sockaddr *)&server, sizeof(server));
    listen(listenfd, 5);

    while (1)
    {
        int connfd = accept(listenfd, (struct sockaddr *)NULL, NULL); //   监听connect()
        read(connfd, info, sizeof(FileInfo));
        FILE *fp = openFile(info->pathname);
        int send_volume = 0, recv_volume = 0;

        for (int readsize = 0; recv_volume < info->size; recv_volume += readsize)
        {
            readsize = read(connfd, tempbuf, TEMPBUFSIZE);
            fwrite(tempbuf, readsize, 1, fp);
        }
        send_volume += write(connfd, &endflag, 4);

        fclose(fp);
        close(connfd);
        printf("\ntotal send %d byte\ntotal recv %d byte\n", send_volume, recv_volume);
    }

    close(listenfd); // Never run
}
