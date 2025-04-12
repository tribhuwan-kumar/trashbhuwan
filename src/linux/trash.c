#include <libgen.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>

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
