#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

const char storepath[32] = "/home/zhouyu/store1/";

typedef struct FileInfo
{
    char pathname[128];
    off_t size;
} FileInfo;

FILE *openFile(char *name)
{
    char path[128] = {0};
    strcat(path, storepath);

    for (int i = strlen(name) - 1;; i--)
        if (name[i] == '/')
        {
            strcat(path, name + i + 1);
            break;
        }

    return fopen(path, "w");
}
