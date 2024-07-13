//<------------------------------CLI Application for managing TRASH wrtitten in 'C'------------------------------------>
// author : @tribhuwan-kumar
// email : trashbhuwan@duck.com
//<-------------------------------------------------------------------------------------------------------------------->
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>

#define YELLOW "\033[0;33m"
#define NC "\033[0m"

// USERNAME
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

// MAKE PATH
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

// MANAGE REL PATH
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

// FORMATTING
void formatting(const char *fileSize, const char *paths, const char *fileName, int sizeWidth, int dirWidth) {
    printf("%-*s %-*s %s\n", sizeWidth, fileSize, dirWidth, paths, fileName);
}

// MAX DIR WIDTH # its necessary to get maximum directory length first
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

// LIST TRASHED FILEs
void listTrashedFiles(const char *trashFilesDir, const char *trashInfoDir) {
    int sizeWidth = 8;
    int dirWidth = getMaxDirWidth(trashFilesDir, trashInfoDir);
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
void createTrashInfo(const char *filePath, const char *trashInfoDir) {
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

// RESTORE TRASH
void restoreTrashedfile(const char *fileName) {
    char filePath[PATH_MAX];
    char infoFilePath[PATH_MAX];
    char originalDirectory[PATH_MAX];
    struct stat st = {0};

    char* trashFilesDir = getPath("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");

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

// RESTORE FILE ON DIFF DESTINATION
void restoreTrashedfileOnDest(const char *fileName, const char *destPath) {
    char filePath[PATH_MAX];
    char infoFilePath[PATH_MAX];
    char fileDestPath[PATH_MAX];

    char* trashFilesDir = getPath("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");

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
    free(trashInfoDir);
    free(trashFilesDir);
}

// MAIN FUNCTION
int main(int argc, char *argv[]) {
    const char* username = getUserName();
    char* trashFilesDir = getPath("/home/", "/.local/share/Trash/files");
    char* trashInfoDir = getPath("/home/", "/.local/share/Trash/info");

    if (username == NULL || trashFilesDir == NULL || trashInfoDir == NULL) {
        perror("Something went wrong");
    }

    // NO ARGUEMENTs
    if (argc == 1) {
        char* printAsciiCommand = "curl -s https://raw.githubusercontent.com/tribhuwan-kumar/trashbhuwan/main/tests/ascii";
        size_t printAsciiCommandSize = strlen(printAsciiCommand) + 1;
        char* printAscii = (char*)malloc(printAsciiCommandSize);
        snprintf(printAscii, printAsciiCommandSize, "%s", printAsciiCommand);
        if (system(printAsciiCommand) != 0) {
            free(printAscii);
        }
        else {
            free(printAscii);
        }
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
        }
        for (int i = 2; i < argc; i++) {
            char *fileNames = argv[i];
            char *resolvedPath = getAbsolutePath(fileNames);
            if (resolvedPath != NULL && access(resolvedPath, F_OK) != -1) {
                char destPath[PATH_MAX];
                snprintf(destPath, sizeof(destPath), "%s/%s", trashFilesDir, basename(resolvedPath));
                if (access(destPath, F_OK) == 0) {
                    int counter = 1;
                    do {
                        snprintf(destPath, sizeof(destPath), "%s/%s-%d", trashFilesDir, basename(resolvedPath), counter);    
                        counter++;
                    }while (access(destPath, F_OK) == 0);
                }
                if (strncmp(resolvedPath, "/mnt", 4) == 0) {
                    char putFileCommand[PATH_MAX];
                    snprintf(putFileCommand, sizeof(putFileCommand), "mv \"%s\" \"%s\"", resolvedPath, destPath);
                    if (system(putFileCommand) == 0) {
                        createTrashInfo(resolvedPath, trashInfoDir); 
                    }
                    else {
                        perror("Failed to trashed file");
                    }
                }
                else if (rename(resolvedPath, destPath) == 0) {
                    createTrashInfo(resolvedPath, trashInfoDir); 
                }
                else {
                    perror("Failed to trash file");
                }
            }
            else {
                fprintf(stderr, "'%s' doesn't exist!!\n", fileNames);
            }
            free(resolvedPath);
        }
    }

    // INDIVIDUALLY DELETE TRASHED FILE
    else if (argc >= 2 && (strcmp(argv[1], "--delete") == 0 || strcmp(argv[1], "-dl") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
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
        }
        for (int i = 2; i < argc; i++) {
            char *fileNames = argv[i];
            restoreTrashedfile(fileNames);
        }
    }
    
    // RESTORE IN SPECIFIED DESTINATION
    else if (argc >= 2 && (strcmp(argv[1], "--restore-dest") == 0 || strcmp(argv[1], "-rd") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
        }
        else if (argc == 3) {
            printf("Please specify a destination path!!\n");
        }
        char *destPath = getAbsolutePath(argv[argc - 1]);
        if (destPath != NULL) {
            for (int i = 2; i < argc - 1; i++) {
                char *fileNames = argv[i];
                restoreTrashedfileOnDest(fileNames, destPath);
            }
        }
        free(destPath);
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
               "    -p,  --put              Put files & directories in trash\n"
               "    -r,  --restore          Restore files & directories from trash\n"
               "    -rd, --restore-dest     Restore files & directories to specific destination\n"
               "    -ls, --list             List trashed files & directories along with disk usage\n"
               "    -dl, --delete           Individually Delete trashed files & directories\n"
               "    -em, --empty            Empty the trash\n"
               "\nReport bugs to: <https://github.com/tribhuwan-kumar/trashbhuwan/issues>\n"
               "'trashbhuwan' home page: <https://github.com/tribhuwan-kumar/trashbhuwan>\n"
               "General more help: <https://github.com/tribhuwan-kumar/trashbhuwan#introduction>\n");
    }

    // INVALID ARGUEMENT
    else {
        printf("Invalid option: %s\n", argv[1]);
        printf("Usage: [--list|-ls] [--put|-p] [--restore|-r] [--delete|-dl] [--empty|-em] [FILE]...[DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
    }

    free(trashInfoDir);
    free(trashFilesDir);
    return 0;
}

