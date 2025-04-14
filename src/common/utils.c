#include <stdio.h>
#include <stdlib.h>

const char *__size_char = "SIZE";
const char *__deleted_from_char = "DELETED FROM";
const char *__files_dirs_char = "FILES & DIRS";
const char *__total_size_char = "TOTAL SIZE";
const char *__dash_char = "------------";

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
