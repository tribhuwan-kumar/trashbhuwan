#if __linux__
#include <libgen.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "./headers.h"
#include "../common/utils.h"

void list_trashed_files(const char *trashFilesDir, const char *trashInfoDir) {
    int sizeWidth = 8;
    int dirWidth = get_max_dir_width(trashFilesDir, trashInfoDir);
    char fileSize[RTSIG_MAX];
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(trashFilesDir)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            char *fileName = entry->d_name;
            char trashInfoFilePath[PATH_MAX];
            snprintf(trashInfoFilePath, PATH_MAX, "%s/%s.trashinfo", trashInfoDir, fileName);

            struct stat info;
            if (stat(trashInfoFilePath, &info) == 0 && (S_ISREG(info.st_mode))) {
                FILE *trashInfoFile = fopen(trashInfoFilePath, "r"); 
                if (trashInfoFile != NULL) {
                    char line[PATH_MAX]; 
                    while (fgets(line, PATH_MAX, trashInfoFile) != NULL) {
                        if (strncmp(line, "Path=", 5) == 0) {
                            char *originalPath = line + 5; 
                            if (originalPath) {
                                // TRASHED FILE DIR AND NAME
                                char originalDir[PATH_MAX]; 
                                snprintf(originalDir, sizeof(originalDir), "%s", dirname(originalPath));
                                // TRASHED FILESIZE
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                                    char trashFilePath[PATH_MAX];
                                    snprintf(trashFilePath, sizeof(trashFilePath), "%s/%s", trashFilesDir, entry->d_name);
                                    size_t commandSize = strlen(trashFilePath) + strlen("du -sh\"\" | cut -f1") + 2;
                                    char* duTrashFilecommand = (char*)malloc(commandSize);
                                    if (duTrashFilecommand == NULL) {
                                        perror("Memory allocation failed");
                                        exit(EXIT_FAILURE);
                                    }
                                    snprintf(duTrashFilecommand, commandSize, "du -sh \"%s\" | cut -f1", trashFilePath);
                                    FILE *fp = popen(duTrashFilecommand, "r"); 
                                    if (fp != NULL) {
                                        if (fgets(fileSize, sizeof(fileSize), fp) != NULL) {
                                            fileSize[strcspn(fileSize, "\n")] = '\0';
                                        }
                                        else {
                                            perror("Failed to read total size");
                                        }
                                        pclose(fp);
                                    }
                                    else {
                                        perror("Failed to run 'du'");
                                        exit(EXIT_FAILURE);
                                    }
                                    free(duTrashFilecommand);
                                }
                                originalDir[strcspn(originalDir, "\n")] = '\0';
                                formatting(fileSize, originalDir, entry->d_name, sizeWidth, dirWidth);
                            }
                        } 
                    }
                    fclose(trashInfoFile);
                }
            }
        }
        closedir(dir);
    }
    else {
        perror("Error opening directory");
    }
}

void create_trash_info(const char *filePath, const char *trashInfoDir) {
    char *filePathCopy = strdup(filePath);

    if (filePathCopy == NULL || trashInfoDir == NULL) {
        perror("Something went wrong!");
        exit(EXIT_FAILURE);
    }

    // GET TIME
    time_t rawtime;
    time(&rawtime);
    char currentTime[30];
    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);

    char *baseName = basename(filePathCopy);
    strftime(currentTime, sizeof(currentTime), "%Y-%m-%dT%H:%M:%S", timeinfo);

    // CREATE '.trashinfo' FILE
    size_t trashInfoFileNameCharSize = strlen(baseName) + strlen(".trashinfo") + 5;
    char* trashInfoFileName = (char*)malloc(trashInfoFileNameCharSize);
    snprintf(trashInfoFileName, trashInfoFileNameCharSize, "%s.trashinfo", baseName);
    
    size_t trashInfoFilePathCharSize = strlen(trashInfoDir) + strlen(trashInfoFileName) + strlen(currentTime) + 2;
    char* trashInfoFilePath = (char*)malloc(trashInfoFilePathCharSize); 
    snprintf(trashInfoFilePath, trashInfoFilePathCharSize, "%s/%s", trashInfoDir, trashInfoFileName);

    if (access(trashInfoFilePath, F_OK) == 0) {
        int counter = 1;
        while (access(trashInfoFilePath, F_OK) == 0) {

            snprintf(trashInfoFileName, trashInfoFileNameCharSize, "%s-%d.trashinfo", baseName, counter);
            snprintf(trashInfoFilePath, trashInfoFilePathCharSize, "%s/%s", trashInfoDir, trashInfoFileName);
            counter++;
        }
    }

    FILE *file = fopen(trashInfoFilePath, "w");
    if (file == NULL) {
        perror("Error in opening trash info file for write");
        exit(EXIT_FAILURE);
    }
    if (ftell(file) == 0) {
        fprintf(file, "[Trash Info]\n"); 
    }
    else {
        fseek(file, 0, SEEK_END);
    }
    fprintf(file, "Path=%s\n", filePath);
    fprintf(file, "DeletionDate=%s\n", currentTime);
    fclose(file);
    free(filePathCopy);
    free(trashInfoFileName);
    free(trashInfoFilePath);
}

void restore_trashed_file(const char *fileName) {
    char filePath[PATH_MAX];
    char infoFilePath[PATH_MAX];
    char originalDirectory[PATH_MAX];
    struct stat st = {0};

    char* trashFilesDir = get_path("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = get_path("/home/", "/.local/share/Trash/info");

    snprintf(filePath, sizeof(filePath), "%s/%s", trashFilesDir, fileName);
    if (access(filePath, F_OK) != -1) {
        snprintf(infoFilePath, sizeof(infoFilePath), "%s/%s.trashinfo", trashInfoDir, fileName);
        if (access(infoFilePath, F_OK) != -1) {
            FILE *infoFile = fopen(infoFilePath, "r");
            if (infoFile) {
                char line[PATH_MAX];
                while (fgets(line, sizeof(line), infoFile)) {
                    if (strncmp(line, "Path=", 5) == 0) {
                        char *originalPath = line + 5;
                        originalPath[strcspn(originalPath, "\n")] = '\0';
                        char *originalPathCopy = strdup(originalPath);
                        strcpy(originalDirectory, dirname(originalPathCopy));
                        free(originalPathCopy);
                        if (stat(originalDirectory, &st) == -1) {
                            if (mkdir(originalDirectory, 0700) == -1) {
                                perror( "Failed to create original directory\n");
                            }
                        }
                        if (strncmp(originalPath, "/mnt", 4) == 0) {
                            char restoreFileCommand[PATH_MAX];
                            snprintf(restoreFileCommand, sizeof(restoreFileCommand), "mv \"%s\" \"%s\"", filePath, originalPath);
                            if (system(restoreFileCommand) == 0) {
                                if (access(infoFilePath, F_OK) == 0) {
                                    remove(infoFilePath);
                                }
                            }
                            else {
                                fprintf(stderr,"Failed to restore %s\n", fileName);
                            }
                        }
                        if (strncmp(originalPath, "/mnt", 4) != 0) {
                            if (rename(filePath, originalPath) == 0) {
                                if (access(infoFilePath, F_OK) == 0) {
                                    remove(infoFilePath);
                                }
                            }
                            else {
                                fprintf(stderr,"Failed to restore %s\n", fileName);
                            }
                            break;
                        }
                    }
                }
                fclose(infoFile);
            }
        }
        else {
            fprintf(stderr, "Couldn't find restoring destination for '%s'\n", fileName);
        }
    }
    else {
        fprintf(stderr, "Couldn't find '%s' in trash!!\n", fileName);
    }
    free(trashInfoDir);
    free(trashFilesDir);
}

void restore_trashed_file_on_dest(const char *fileName, const char *destPath) {
    char filePath[PATH_MAX];
    char infoFilePath[PATH_MAX];
    char fileDestPath[PATH_MAX];

    char* trashFilesDir = get_path("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = get_path("/home/", "/.local/share/Trash/info");

    snprintf(filePath, sizeof(filePath), "%s/%s", trashFilesDir, fileName);
    snprintf(infoFilePath, sizeof(infoFilePath), "%s/%s.trashinfo", trashInfoDir, fileName);

    if (access(filePath, F_OK) != -1) {
        if (access(destPath, F_OK) != -1) {
            snprintf(fileDestPath, sizeof(fileDestPath), "%s/%s", destPath, fileName);
            if (strncmp(fileDestPath, "/mnt", 4) == 0) {
                char restoreFileCommand[PATH_MAX];
                snprintf(restoreFileCommand, sizeof(restoreFileCommand), "mv \"%s\" \"%s\"", filePath, fileDestPath);
                if (system(restoreFileCommand) == 0) {
                    if (access(infoFilePath, F_OK) == 0) {
                        remove(infoFilePath);
                    }
                }
                else {
                    fprintf(stderr,"Failed to restore '%s'\n", fileName);
                }
            }
            if (strncmp(fileDestPath, "/mnt", 4) != 0) {
                if (rename(filePath, fileDestPath) == 0) {
                    if (access(infoFilePath, F_OK) == 0) {
                        remove(infoFilePath);
                    }
                }
                else {
                    fprintf(stderr,"Failed to restore '%s'\n", fileName);
                }
            }
        }
        else {
            fprintf(stderr, "Not a valid destination '%s'\n", destPath);
        }
    }
    else {
        fprintf(stderr, "Couldn't find '%s' in trash!!\n", fileName);
    }
    free(trashInfoDir);
    free(trashFilesDir);
}

void delete_trashed_file(const char *fileName){
    char* trashFilesDir = get_path("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = get_path("/home/", "/.local/share/Trash/info");

    char trashedFilePath[PATH_MAX];
    char trashInfoFilePath[PATH_MAX];

    snprintf(trashedFilePath, sizeof(trashedFilePath), "%s/%s", trashFilesDir, fileName);
    snprintf(trashInfoFilePath, sizeof(trashInfoFilePath), "%s/%s.trashinfo", trashInfoDir, fileName);

    if (access(trashedFilePath, F_OK) == 0 && access(trashInfoFilePath, F_OK) == 0) {
        if (remove(trashedFilePath) != 0) {
            if (rmdir(trashedFilePath) != 0) {
                char deleteCommand[PATH_MAX + 10];
                snprintf(deleteCommand, sizeof(deleteCommand), "rm -rf \"%s\"", trashedFilePath);
                if (system(deleteCommand) != 0) {
                    perror("Error in deleting file");
                }
            }
        }
        if (remove(trashInfoFilePath) != 0) {
            perror("Error deleting file");
        }
    }
    else {
        fprintf(stderr, "'%s' not found in trash!!\n", basename(trashedFilePath));
    }
    free(trashInfoDir);
    free(trashFilesDir);
}

void empty_trash(const char *trash_files_dir, const char *trash_info_dir) {
    if (is_directory_empty(trash_files_dir) && is_directory_empty(trash_info_dir)) {
        exit(EXIT_SUCCESS);
    }
    if (!ask_confirmation()) {
        exit(EXIT_SUCCESS);
    }
    
    DIR *dir = opendir(trash_files_dir);
    if (dir == NULL) {
        perror("Error in opening trash file directory"); 
        exit(EXIT_FAILURE); 
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char *filePath = malloc(PATH_MAX);
        if (filePath == NULL) {
            perror("Error allocating memory for file path"); 
            exit(EXIT_FAILURE);
        }
        snprintf(filePath, PATH_MAX, "%s/%s", trash_files_dir, entry->d_name);
        if (remove(filePath) != 0) {
            if (rmdir(filePath) != 0) {
                char deleteCommand[PATH_MAX + 10];
                snprintf(deleteCommand, sizeof(deleteCommand), "rm -rf \"%s\"", filePath);
                if (system(deleteCommand) != 0) {
                    perror("Error in deleting files");
                }
            }
        }
        free(filePath);
    }
    closedir(dir);

    dir = opendir(trash_info_dir);
    if (dir == NULL) {
        perror("Error in opening trash info directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", trash_info_dir, entry->d_name);

        if (remove(filePath) != 0) {
            perror("Error deleting file");
        }
    }
    closedir(dir);
}
#endif
