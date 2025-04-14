#ifdef _WIN32
#include <stdio.h>
#include <stdint.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windows.h>
#include <stdlib.h>
#include <wchar.h>

#include "./headers.h"

void list_recycle_bin() {
    IFileMetadata *metadataArray = NULL;
    int metadataCount = 0;
    int metadataCapacity = 10;

    metadataArray = (IFileMetadata *)malloc(metadataCapacity * sizeof(IFileMetadata));
    if (!metadataArray) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    int count = 0;
    char **RFilePaths = list_$R(&count);

    if (RFilePaths) {
        for (int i = 0; i < count; i++) {
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char filePath[MAX_PATH];
            // replace $R with $I for decoding metadata
            const char *filename = PathFindFileName(RFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - RFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), RFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(filePath, sizeof(filePath), dirName, _TRUNCATE);
            strncat_s(filePath, sizeof(filePath), fileName, _TRUNCATE);

            IFileMetadata metadata = decode_metadata(filePath);

            create_metadata_arr(&metadataArray, &metadataCount, &metadataCapacity, metadata);
        }
        free(RFilePaths);
    } else {
        printf("Trash is empty!!\n");
    }

    print_metadata(metadataArray, metadataCount);

    // cleanup
    free(metadataArray);
}

void restore_file(const char *fileNameArg) {
    int count = 0;
    char **RFilePaths = list_$R(&count);

    if (RFilePaths) {
        for (int i = 0; i < count; i++) {
            // get the $I
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char filePath[MAX_PATH];
            const char *filename = PathFindFileName(RFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - RFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), RFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(filePath, sizeof(filePath), dirName, _TRUNCATE);
            strncat_s(filePath, sizeof(filePath), fileName, _TRUNCATE);
            // decode the $I
            IFileMetadata metadata = decode_metadata(filePath);
            if (strcmp(metadata.fileName, fileNameArg) == 0) {
                // restore file using SHFileOperation
                SHFILEOPSTRUCT fileOp = {0};
                fileOp.wFunc = FO_MOVE;
                fileOp.pFrom = RFilePaths[i];
                fileOp.pTo = metadata.utf8Path;
                fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI;

                if (SHFileOperation(&fileOp) != 0) {
                    fprintf(stderr, "Failed to restore: %s\n", fileName);
                }
                break;
            }  
        }
    } else {
        printf("Trash is empty!!\n");
    }
    free(RFilePaths);
}

void restore_file_to_dest(const char *fileNameArg, const char *destPath) {
    int count = 0;
    char **RFilePaths = list_$R(&count);

    if (RFilePaths) {
        for (int i = 0; i < count; i++) {
            // get the $I
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char filePath[MAX_PATH];
            const char *filename = PathFindFileName(RFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - RFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), RFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(filePath, sizeof(filePath), dirName, _TRUNCATE);
            strncat_s(filePath, sizeof(filePath), fileName, _TRUNCATE);
            // decode the $I
            IFileMetadata metadata = decode_metadata(filePath);
            if (strcmp(metadata.fileName, fileNameArg) == 0) {
                char filePath[MAX_PATH];
                strcpy_s(filePath, sizeof(filePath), destPath);
                strncpy_s(filePath, sizeof(filePath), destPath, _TRUNCATE);
                strncat_s(filePath, sizeof(filePath), metadata.fileName, _TRUNCATE);
                // restore file using SHFileOperation
                SHFILEOPSTRUCT fileOp = {0};
                fileOp.wFunc = FO_MOVE;
                fileOp.pFrom = RFilePaths[i];
                fileOp.pTo = filePath;
                fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI;

                if (SHFileOperation(&fileOp) != 0) {
                    fprintf(stderr, "Failed to restore: %s\n", fileName);
                }
                break;
            } 
        }
    } else {
        printf("Trash is empty!!\n");
    }
    free(RFilePaths);
}

void empty_trash(const char *file_path) {
    /* empty_trash trash */
}

int put_file(const char *filePath) {
    char *path = NULL;
    size_t pathLen = strlen(filePath);

    if (pathLen >= MAX_PATH) {
        path = (char *)malloc(pathLen + 5); // null terminator included
        if (!path) {
            fprintf(stderr, "Memory allocation failed!!\n");
            return 1;
        }
        snprintf(path, pathLen + 5, "\\\\?\\%s", filePath);
    } else {
        static char localPath[MAX_PATH + 1];
        path = localPath;
        snprintf(path, MAX_PATH + 1, "%s", filePath);
    }
    
    size_t pathWithNullLen = strlen(path) + 2;
    char *pathWithNull = (char *)malloc(pathWithNullLen);

    if (!pathWithNull) {
        fprintf(stderr, "Memory allocation failed!!\n");
        if (path != filePath) free(path);
        return 1;
    }
    snprintf(pathWithNull, pathWithNullLen, "%s", path);
    pathWithNull[pathWithNullLen - 1] = '\0'; // double null-terminate

    // prepare SHFILEOPSTRUCT
    SHFILEOPSTRUCT fileOp = {0};
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = pathWithNull;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

    if (SHFileOperation(&fileOp) == 0) {
        return 0; 
    } else {
        fprintf(stderr, "Failed to put '%s' in recycle bin!!\n", filePath);
        return 1; 
    }

    // cleanup
    if (path != filePath) free(path);
    free(pathWithNull);
}

void delete_file(const char *file_path) {
    if (DeleteFile(file_path)) {
        printf("Deleted: %s\n", file_path);
    } else {
        printf("Failed to delete: %s\n", file_path);
    }
}



#endif
