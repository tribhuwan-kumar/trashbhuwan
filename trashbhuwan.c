//<------------------------------CLI Application for managing TRASH wrtitten in 'C'------------------------------------>
// author : @tribhuwan-kumar
// email : trashbhuwan@proton.me
//<-------------------------------------------------------------------------------------------------------------------->
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __linux__
    #include <libgen.h>
    #include <dirent.h>
    #include <unistd.h>
    #include "src/linux/headers.h"
#endif

#ifdef _WIN32
    #include <io.h>
    #include "src/windows/headers.h"
#endif

#define YELLOW "\033[0;33m"
#define NC "\033[0m"

int main(int argc, char *argv[]) {
    #if __linux__
        const char* username = get_user_name();
        char* trashFilesDir = get_path("/home/", "/.local/share/Trash/files");
        char* trashInfoDir = get_path("/home/", "/.local/share/Trash/info");

        if (username == NULL || trashFilesDir == NULL || trashInfoDir == NULL) {
            perror("Something went wrong");
        }
    #endif

    // no arguementS
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

    // list
    else if (argc == 2 && (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-ls") == 0 )) {
        const char *size = "SIZE";
        const char *deleted_from = "DELETED FROM";
        const char *files_dirs = "FILES & DIRS";
        const char *total_size = "TOTAL SIZE";
        const char *dash = "------------";
        #if __linux__
            char totalSize[RTSIG_MAX];

            int sizeWidth = 8;
            int dirWidth = get_max_dir_width(trashFilesDir, trashInfoDir);

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
                } else {
                    perror("Failed to read total size");
                }
                pclose(fp);
            } else {
                perror("Failed to run 'du'");
                exit(EXIT_FAILURE);
            }
            free(duTrashDircommand);

            if (is_directory_empty(trashFilesDir) || is_directory_empty(trashInfoDir)) {
                printf("Trash is empty!!\n");
            } else {
                printf("%s%-*s%s %s%-*s%s %s%-s%s\n", YELLOW, sizeWidth, size, NC, YELLOW, dirWidth, deleted_from, NC, YELLOW, files_dirs, NC);
                list_trashed_files(trashFilesDir, trashInfoDir);
                printf("%s%-*s%s %s%-*s%s %s%-s%s\n", YELLOW, sizeWidth, totalSize, NC, YELLOW, dirWidth, total_size, NC, YELLOW, dash, NC);
            }
        #endif
        
        #ifdef _WIN32
            list_recycle_bin_items();
        #endif
    } 
    
    // put
    else if (argc >= 2 && (strcmp(argv[1], "--put") == 0 || strcmp(argv[1], "-p") == 0 )) { 
        if (argc == 2) {
            printf("No input files were provided!!\n");
        }
        #if __linux__
            for (int i = 2; i < argc; i++) {
                char *fileNames = argv[i];
                char *resolvedPath = get_absolute_path(fileNames);
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
                            create_trash_info(resolvedPath, trashInfoDir); 
                        } else {
                            perror("Failed to trashed file");
                        }
                    } else if (rename(resolvedPath, destPath) == 0) {
                        create_trash_info(resolvedPath, trashInfoDir); 
                    } else {
                        perror("Failed to trash file");
                    }
                } else {
                    fprintf(stderr, "'%s' doesn't exist!!\n", fileNames);
                }
                free(resolvedPath);
            }
        #endif
        #ifdef _WIN32
            for (int i = 2; i < argc; i++) {
                char *fileName = argv[i];
                if (_access(fileName, 0) == 0) {
                    put_file(fileName);
                } else {
                    fprintf(stderr, "'%s' doesn't exist!!\n", fileName);
                };
            }
        #endif
    } 
    
    // delete
    else if (argc >= 2 && (strcmp(argv[1], "--delete") == 0 || strcmp(argv[1], "-dl") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
        }
        #if __linux__
            for (int i = 2; i < argc; i++) {
                char *fileNames = argv[i];
                delete_trashed_file(fileNames);
            }
        #endif
    } 
    
    // restore
    else if (argc >= 2 && (strcmp(argv[1], "--restore") == 0 || strcmp(argv[1], "-r") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
        }
        #if __linux__
            for (int i = 2; i < argc; i++) {
                char *fileNames = argv[i];
                restore_trashed_file(fileNames);
            }
        #endif
    } 

    // restore to specific destination
    else if (argc >= 2 && (strcmp(argv[1], "--restore-dest") == 0 || strcmp(argv[1], "-rd") == 0 )) {
        if (argc == 2) {
            printf("No input files were provided!!\n");
        }
        else if (argc == 3) {
            printf("Please specify a destination path!!\n");
        }
        #if __linux__
            char *destPath = get_absolute_path(argv[argc - 1]);
            if (destPath != NULL) {
                for (int i = 2; i < argc - 1; i++) {
                    char *fileNames = argv[i];
                    restore_trashed_file_on_dest(fileNames, destPath);
                }
            }
            free(destPath);
        #endif
    } 

    // empty trash
    else if (argc == 2 && strcmp(argv[1], "--empty") == 0 || strcmp(argv[1], "-em") == 0) {
        #if __linux__
            empty_trash(trashFilesDir, trashInfoDir);
        #endif
    } 

    // help
    else if (strcmp(argv[1], "-h") == 0) {
        printf("Usage: trashbhuwan --put [FILE] [DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
    } 

    // this is also help
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

    // when passing invalid options
    else {
        printf("Invalid option: %s\n", argv[1]);
        printf("Usage: [--list|-ls] [--put|-p] [--restore|-r] [--delete|-dl] [--empty|-em] [FILE]...[DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
    }
    #if __linux__
        free(trashInfoDir);
        free(trashFilesDir);
    #endif
    return 0;
}

