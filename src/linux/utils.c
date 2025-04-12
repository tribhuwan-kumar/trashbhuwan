#include <libgen.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "src/common/utils.h"

int isDirectoryEmpty(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    int isEmpty = 1;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        isEmpty = 0;
        break;
    }
    closedir(dir);
    return isEmpty;
}

const char* getUserName() {
    const char *username = getenv("LOGNAME");
    if (username == NULL) {
        username = getenv("USER");
        if (username == NULL) {
            perror("Couldn't get the Username");
            return NULL;
        }
    }
    return username;
}

char* getPath(const char* base, const char* suffix){
    const char* username = getUserName();
    if (username == NULL) {
        return NULL;
    }
    size_t pathSize = strlen(base) + strlen(username) + strlen(suffix) + 1;
    char* path = (char*)malloc(pathSize);
    if (path == NULL) {
        perror("Memory allocation error in 'getPath()'");
        free(path);
        exit(EXIT_FAILURE);
    }
    snprintf(path, pathSize, "%s%s%s", base, username, suffix);
    return path;
}

char* getAbsolutePath(const char* relPath) {
    char* absPath = malloc(PATH_MAX);
    if (absPath == NULL) {
        perror("Memory allocation failed in 'getAbsolutePath()'");
        exit(EXIT_FAILURE);
    }
    if (relPath[0] == '/') {
        strcpy(absPath, relPath);
    } 
    else {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("Failed to get current working directory");
            free(absPath);
            exit(EXIT_FAILURE);
        }
        snprintf(absPath, PATH_MAX, "%s/%s", cwd, relPath);
    }
    return absPath;
}

int getMaxDirWidth(const char *trashFilesDir, const char *trashInfoDir){
    int dirWidth = 0;
    int sizeWidth = 8;  

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(trashFilesDir)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            char *fileName = entry->d_name;
            char trashInfoFileName[PATH_MAX];
            snprintf(trashInfoFileName, PATH_MAX, "%s/%s.trashinfo", trashInfoDir, fileName);
            struct stat info;
            if (stat(trashInfoFileName, &info) == 0) {
                if (S_ISREG(info.st_mode)) { 
                    FILE *trashInfoFile = fopen(trashInfoFileName, "r"); 
                    if (trashInfoFile != NULL) {
                        char line[PATH_MAX]; 
                        while (fgets(line, PATH_MAX, trashInfoFile) != NULL) {
                            if (strncmp(line, "Path=", 5) == 0) {
                                char *originalPath = line + 5; 
                                if (originalPath) {
                                    if (originalPath) {
                                        char originalDir[PATH_MAX]; 
                                        snprintf(originalDir, sizeof(originalDir), "%s", dirname(originalPath));
                                        if (strlen(originalDir) > dirWidth) {
                                            dirWidth = strlen(originalDir);
                                        }
                                    }
                                }
                            } 
                        }
                        fclose(trashInfoFile);
                    }
                }
            }
        }
        closedir(dir);
        dirWidth += 8;
    }
    else {
        perror("Error opening directory");
    }
    return dirWidth;
}

