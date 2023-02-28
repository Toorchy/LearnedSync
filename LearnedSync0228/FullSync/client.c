#include "fileinfo.h"
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 7789
#define TEMPBUFSIZE 1024 * 33

int main(int argc, char const *argv[])
{
    char tempbuf[TEMPBUFSIZE] = {0};

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {AF_INET, htons(PORT), *((struct in_addr *)gethostbyname(argv[1])->h_addr)};
    connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr));

    int flag, send_volume, recv_volume;
    struct timeval startTime, endTime;
    FILE *fp = fopen(argv[2], "r");

    struct stat st;
    stat(argv[2], &st);
    FileInfo info = {{0}, st.st_size};
    strcpy(info.pathname, argv[2]);

    gettimeofday(&startTime, NULL);

    send_volume += write(fd, &info, sizeof(FileInfo));

    for (int readsize = 0; (readsize = fread(tempbuf, 1, TEMPBUFSIZE, fp)) > 0;)
        send_volume += write(fd, tempbuf, readsize);

    recv_volume += read(fd, &flag, 4);

    fclose(fp);
    close(fd);

    gettimeofday(&endTime, NULL);

    float totalTime = ((endTime.tv_sec - startTime.tv_sec) * 1000000 +
                       (endTime.tv_usec - startTime.tv_usec));

    printf("totalTime is %f s\ntotal send %d byte\ntotal recv %d byte\n",
           totalTime / 1000000, send_volume, recv_volume);
}
