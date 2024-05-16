//<------------------------------CLI Application for managing TRASH wrtitten in 'C'------------------------------------>
// author : @tribhuwan-kumar
// email : freakybytes@duck.com
//<-------------------------------------------------------------------------------------------------------------------->
#include <time.h>
#include <glob.h>
#include <stdio.h>
#include <iso646.h>
#include <stdlib.h>
#include <libgen.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <linux/limits.h>

#define YELLOW "\033[0;33m"
#define NC "\033[0m"

// USERNAME
const char* getUserName() {
    const char *username = getenv("LOGNAME");
    if (username == NULL) {
        username = getenv("USER");
        if (username == NULL) {
            printf("Could not get username.\n");
            return NULL;
        }
    }
    return username;
}

// PATH
char* getPath(const char* base, const char* suffix){
    const char* username = getUserName();
    if (username == NULL) {
        return NULL;
    }
    size_t pathSize = strlen(base) + strlen(username) + strlen(suffix) +1;
    char* path = (char*)malloc(pathSize);
    if (path == NULL) {
        fprintf(stderr, "Memory allocation error in 'getPath'");
        return NULL;
    }
    snprintf(path, pathSize, "%s%s%s", base, username, suffix);
    return path;
}

// MANAGE ABSOLUTE PATH
char* getAbsolutePath(const char* relPath) {
    char* absPath = malloc(PATH_MAX);
    if (absPath == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    if (relPath[0] == '/') {
        strcpy(absPath, relPath);
    } 
    else {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "Failed to get current working directory\n");
            free(absPath);
            exit(EXIT_FAILURE);
        }
        snprintf(absPath, PATH_MAX, "%s/%s", cwd, relPath);
    }
    return absPath;
}

// FORMATTING
void formatting(const char *fileSize, const char *paths, const char *fileName, int sizeWidth, int dirWidth) {
    printf("%-*s %-*s %s\n", sizeWidth, fileSize, dirWidth, paths, fileName);
}

// MAX DIR WIDTH # its necessary to get maximum directory length first
int getMaxDirWidth(const char *trashFilesDir, const char *trashInfoDir){
    int dirWidth;
    int sizeWidth = 8;  

    DIR *dir;
    struct dirent *entry;
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Curl initialization failed!\n");
        exit(EXIT_FAILURE);
    }

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
                                char *originalEncodedPath = line + 5; 
                                if (originalEncodedPath != NULL) {
                                    char *originalPath = curl_easy_unescape(curl, originalEncodedPath, 0, NULL);
                                    if (originalPath) {
                                        char originalDir[PATH_MAX]; 
                                        snprintf(originalDir, sizeof(originalDir), "%s", dirname(originalPath));
                                        if (strlen(originalDir) > dirWidth) {
                                            dirWidth = strlen(originalDir);
                                        }
                                        curl_free(originalPath);
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
    curl_easy_cleanup(curl);
    return dirWidth;
}

// LIST TRASHED FILEs
void listTrashedFiles(const char *trashFilesDir, const char *trashInfoDir) {
    int sizeWidth = 8;
    int dirWidth = getMaxDirWidth(trashFilesDir, trashInfoDir);
    char fileSize[RTSIG_MAX];
    DIR *dir;
    struct dirent *entry;

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Curl initialization failed!\n");
        exit(EXIT_FAILURE);
    }

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
                            char *originalEncodedPath = line + 5; 
                            if (originalEncodedPath != NULL) {
                                char *originalPath = curl_easy_unescape(curl, originalEncodedPath, 0, NULL);
                                if (originalPath) {
                                    // TRASHED FILE DIR AND NAME
                                    char originalDir[PATH_MAX]; 
                                    char fileName[FILENAME_MAX];
                                    char *originalPathCopy = strdup(originalPath);
                                    snprintf(originalDir, sizeof(originalDir), "%s", dirname(originalPath));
                                    snprintf(fileName, sizeof(fileName), "%s", basename(originalPathCopy));
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
                                    fileName[strcspn(fileName, "\n")] = '\0';
                                    originalDir[strcspn(originalDir, "\n")] = '\0';
                                    formatting(fileSize, originalDir, fileName, sizeWidth, dirWidth);
                                    curl_free(originalPath);
                                }
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

// CHECK EMPTY DIR
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

// ASK FOR CONFIRMATION
int askConfirmation() {
    printf("Are you sure you want to empty the trash? This action can't be undone. [y/N] ");
    char response;
    scanf("%c", &response);
    if (response == 'y' || response == 'Y') {
        return 1;
    } 
    else {
        exit(EXIT_SUCCESS);
    }
}

// EMPTY THE TRASH
void emptyTrash(const char *trash_files_dir, const char *trash_info_dir) {
    if (isDirectoryEmpty(trash_files_dir) && isDirectoryEmpty(trash_info_dir)) {
        exit(EXIT_SUCCESS);
    }
    if (!askConfirmation()) {
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

// CREATE TRASHINFO FILE
void createTrashInfo(const char *filePath) {
    char *filePathCopy = strdup(filePath);
    if (filePathCopy == NULL) {
        perror("Failed to duplicate 'filePath'"); 
        exit(EXIT_FAILURE);
    }
    char *baseName = basename(filePathCopy);

    // GET TIME
    time_t rawtime;
    time(&rawtime);
    char currentTime[30];
    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);
    strftime(currentTime, sizeof(currentTime), "%Y-%m-%dT%H:%M:%S", timeinfo);

    // ENCODE THE FILE PATH
    char *encodedPath = NULL;
    CURL *curl = curl_easy_init(); 
    if (curl) {
        encodedPath = curl_easy_escape(curl, filePath, 0);
        if (encodedPath == NULL) {
            fprintf(stderr, "Failed to encode URL in createTrashInfo\n"); 
            exit(EXIT_FAILURE);
        }
    } 
    else {
        fprintf(stderr, "CURL initialization failed\n");
        exit(EXIT_FAILURE);
    }
    // CREATE '.trashinfo' FILE
    size_t trashInfoFileNameCharSize = strlen(baseName) + strlen(".trashinfo") + 1;
    char* trashInfoFileName = (char*)malloc(trashInfoFileNameCharSize);
    sprintf(trashInfoFileName, "%s.trashinfo", baseName);

    // WRITE ENCODED PATH
    const char* username = getUserName();
    if (username == NULL) {
        exit(EXIT_FAILURE);
    }
    char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");
    if (trashInfoDir == NULL) {
        exit(EXIT_FAILURE);
    }
    size_t trashInfoFilePathCharSize = strlen(trashInfoDir) + strlen(trashInfoFileName) + 2;
    char* trashInfoFilePath = (char*)malloc(trashInfoFilePathCharSize); 
    if (!trashInfoFilePath) {
        perror("Failed to allocate memory for trashInfoFilePath"); 
        exit(EXIT_FAILURE);
    }
    sprintf(trashInfoFilePath, "%s/%s", trashInfoDir, trashInfoFileName);

    FILE *file = fopen(trashInfoFilePath, "w");
    if (file == NULL) {
        perror("Error opening trash info file for writing");
        exit(EXIT_FAILURE);
    }
    if (ftell(file) == 0) {
        fprintf(file, "[Trash Info]\n"); 
    } 
    else {
        fseek(file, 0, SEEK_END);
    }
    fprintf(file, "Path=%s\n", encodedPath);
    fprintf(file, "DeletionDate=%s\n", currentTime);

    fclose(file);
    free(trashInfoFileName);
    free(trashInfoFilePath);
    curl_easy_cleanup(curl);
    if (encodedPath) {
        curl_free(encodedPath); 
    }
}

// RESTORE TRASH
void restoreTrashedfile(const char *fileName) {
    char filePath[PATH_MAX];
    char infoFilePath[PATH_MAX];
    char originalDirectory[PATH_MAX];
    struct stat st = {0};

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Curl initialization failed!\n");
        exit(EXIT_FAILURE);
    }
    char* trashFilesDir = getPath("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");

    snprintf(filePath, sizeof(filePath), "%s/%s", trashFilesDir, fileName);
    if (access(filePath, F_OK) != -1) {
        snprintf(infoFilePath, sizeof(infoFilePath), "%s/%s.trashinfo", trashInfoDir, fileName);
        glob_t globResult;
        glob(infoFilePath, GLOB_TILDE, NULL, &globResult);
        if (globResult.gl_pathc > 0) {
            strcpy(infoFilePath, globResult.gl_pathv[0]);
            FILE *infoFile = fopen(infoFilePath, "r");
            if (infoFile) {
                char line[PATH_MAX];
                while (fgets(line, sizeof(line), infoFile)) {
                    if (strncmp(line, "Path=", 5) == 0) {
                        char *originalEncodedPath = line + 5;
                        if (originalEncodedPath != NULL){
                            char *originalPath = curl_easy_unescape(curl, originalEncodedPath, 0, NULL);
                            originalPath[strcspn(originalPath, "\n")] = '\0';
                            char *originalPathCopy = strdup(originalPath);
                            strcpy(originalDirectory, dirname(originalPathCopy));
                            if (stat(originalDirectory, &st) == -1) {
                                if (mkdir(originalDirectory, 0700) == -1) {
                                    fprintf(stderr, "Failed to create original directory\n");
                                    exit(EXIT_FAILURE);
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
                                    printf("Failed to restore %s\n", fileName);
                                }
                            }
                            if (strncmp(originalPath, "/mnt", 4) != 0) {
                                if (rename(filePath, originalPath) == 0) {
                                    if (access(infoFilePath, F_OK) == 0) {
                                        remove(infoFilePath);
                                    }
                                }
                                else {
                                    printf("Failed to restore %s\n", fileName);
                                }
                                break;
                            }
                        }
                    }
                }
                fclose(infoFile);
            }
            globfree(&globResult);
        }
        else {
            printf("Couldn't find restoring destination for '%s'\n", fileName);
        }
    }
    else {
        printf("Couldn't find '%s' in trash\n", fileName);
    }
}

// INDIVIDUALLY DELETE TRASHED FILE
void deleteTrashedFile(const char *fileName){
    char* trashFilesDir = getPath("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");

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
}

// MAIN FUNCTION
int main(int argc, char *argv[]) {
    const char* username = getUserName();
    const char* trashFilesDir = getPath("/home/", "/.local/share/Trash/files");
    const char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");

    if (username == NULL && trashFilesDir == NULL && trashInfoDir == NULL) {
        perror("Something went wrong");
        return 1;
    }

    // NO ARGUEMENTs
    if (argc == 1) {
        printf("No arguments were provided, See the github page https://github.com/tribhuwan-kumar/trashbhuwan for usage.\n");
        return 1;
    }

    // LIST TRASH
    else if (argc == 2 && (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-ls") == 0 )) {
        char totalSize[RTSIG_MAX];
        const char *size = "SIZE";
        const char *deleted_from = "DELETED FROM";
        const char *files_dirs = "FILES & DIRS";
        const char *total_size = "TOTAL SIZE";
        const char *dash = "------------";

        int sizeWidth = 8;
        int dirWidth = getMaxDirWidth(trashFilesDir, trashInfoDir);

        // GET TOTAL SIZE
        size_t commandSize = strlen(trashFilesDir) + strlen("du -sh\"\" | cut -f1") + 2;
        char* duTrashDircommand = (char*)malloc(commandSize);
        if (duTrashDircommand == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        snprintf(duTrashDircommand, commandSize, "du -sh %s | cut -f1", trashFilesDir);
        FILE *fp = popen(duTrashDircommand, "r"); 
        if (fp != NULL) {
            if (fgets(totalSize, sizeof(totalSize), fp) != NULL) {
                totalSize[strcspn(totalSize, "\n")] = '\0';
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
        free(duTrashDircommand);

        // FORMAT TRASH
        if (isDirectoryEmpty(trashFilesDir) || isDirectoryEmpty(trashInfoDir)) {
            printf("Trash is empty!!\n");
        }
        else {
            printf("%s%-*s%s %s%-*s%s %s%-s%s\n", YELLOW, sizeWidth, size, NC, YELLOW, dirWidth, deleted_from, NC, YELLOW, files_dirs, NC);
            listTrashedFiles(trashFilesDir, trashInfoDir);
            printf("%s%-*s%s %s%-*s%s %s%-s%s\n", YELLOW, sizeWidth, totalSize, NC, YELLOW, dirWidth, total_size, NC, YELLOW, dash, NC);
        }
    }

    // PUT FILEs IN TRASH
    else if (argc >= 2 && (strcmp(argv[1], "--put") == 0 || strcmp(argv[1], "-p") == 0 )) { 
        if (argc == 2) {
            printf("No input files were provided!!\n");
            return 1;
        }
        for (int i = 2; i < argc; i++) {
            char *fileNames = argv[i];
            char *resolvedPath = getAbsolutePath(fileNames);
            if (resolvedPath != NULL) {
                if (access(resolvedPath, F_OK) != -1) {
                    char destPath[PATH_MAX];
                    snprintf(destPath, sizeof(destPath), "%s/%s", trashFilesDir, basename(resolvedPath));
                    if (strncmp(resolvedPath, "/mnt", 4) == 0) {
                        char putFileCommand[PATH_MAX];
                        snprintf(putFileCommand, sizeof(putFileCommand), "mv \"%s\" \"%s\"", resolvedPath, destPath);
                        if (system(putFileCommand) == 0) {
                            createTrashInfo(resolvedPath); 
                            free(resolvedPath);
                            exit(EXIT_SUCCESS);
                        }
                        else {
                            perror("Failed to trashed file");
                            exit(EXIT_FAILURE);
                        }
                    }
                    if (rename(resolvedPath, destPath) == 0) {
                        createTrashInfo(resolvedPath); 
                        free(resolvedPath);
                    }
                    else {
                        perror("Failed to trashed file");
                    }
                }
                else {
                    printf("%s doesn't exist!!\n", fileNames);
                }
            }
        }
    } 

    // INDIVIDUALLY DELETE TRASHED FILE
    else if (argc >= 2 && (strcmp(argv[1], "--delete") == 0 || strcmp(argv[1], "-dl") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
            return 1;
        }
        for (int i = 2; i < argc; i++) {
            char *fileNames = argv[i];
            deleteTrashedFile(fileNames);
        }
    }

    // RESTORE TRASH
    else if (argc >= 2 && (strcmp(argv[1], "--restore") == 0 || strcmp(argv[1], "-r") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
            return 1;
        }
        for (int i = 2; i < argc; i++) {
            char *fileNames = argv[i];
            restoreTrashedfile(fileNames);
        }
    }

    // EMPTY TRASH
    else if (argc == 2 && strcmp(argv[1], "--empty") == 0 || strcmp(argv[1], "-em") == 0) {
        emptyTrash(trashFilesDir, trashInfoDir);
    }

    // HELP
    else if (strcmp(argv[1], "-h") == 0) {
        printf("Usage: trashbhuwan --put [FILE] [DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
    }
    else if (strcmp(argv[1], "--help") == 0) {
        printf("Usage: trashbhuwan [OPTION] [FILE]...\n"
               "Trashing & Restoring files\n\n"
               "Usage: trashbhuwan --put [FILE] [DIRECTORY]\n"
               "Puts file and directory in trash\n\n"
               "Available flags:\n"
               "    -p,  --put          Put files & directories in trash\n"
               "    -r,  --restore      Restore files & directories from the trash\n"
               "    -ls, --list         List trashed files & directories along with disk usage\n"
               "    -dl, --delete       Individually Delete trashed files & directories\n"
               "    -em, --empty        Empty the trash\n"
               "\nReport bugs to: <https://github.com/tribhuwan-kumar/trashbhuwan/issues>\n"
               "'trashbhuwan' home page: <https://github.com/tribhuwan-kumar/trashbhuwan>\n"
               "General more help: <https://github.com/tribhuwan-kumar/trashbhuwan#introduction>\n");
    }

    // INVALID ARGUEMENT
    else {
        printf("Invalid option: %s\n", argv[1]);
        printf("Usage: [--list|-ls] [--put|-p] [--restore|-r] [--delete|-dl] [--empty|-em] [FILE]...[DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
        return 1;
    }

    return 0;
}

