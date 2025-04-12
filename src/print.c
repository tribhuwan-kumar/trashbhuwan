#include <stdio.h>

void formatting(const char *fileSize, const char *paths, const char *fileName, int sizeWidth, int dirWidth) {
    printf("%-*s %-*s %s\n", sizeWidth, fileSize, dirWidth, paths, fileName);
}
