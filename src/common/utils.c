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
