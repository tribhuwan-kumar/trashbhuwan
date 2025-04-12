#include <stdio.h>
#include <stdlib.h>

int ask_confirmation() {
    printf("Are you sure you want to empty the trash? This action can't be undone. [y/N] ");
    char response;
    scanf("%c", &response);
    if (response == 'y' || response == 'Y') {
        return 1;
    } 
    else {
        exit(0);
    }
}

void formatting(const char *fileSize, const char *paths, const char *fileName, int sizeWidth, int dirWidth) {
    printf("%-*s %-*s %s\n", sizeWidth, fileSize, dirWidth, paths, fileName);
}
