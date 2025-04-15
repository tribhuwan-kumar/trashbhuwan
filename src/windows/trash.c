#ifdef _WIN32
#include <stdio.h>
#include <stdint.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windows.h>
#include <stdlib.h>
#include <wchar.h>

#include "./headers.h"
#include "../common/utils.h"

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
    char **rFilePaths = list_$R(&count);

    if (rFilePaths) {
        for (int i = 0; i < count; i++) {
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char iFilePath[MAX_PATH];
            // replace $R with $I for decoding metadata
            const char *filename = PathFindFileName(rFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - rFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), rFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(iFilePath, sizeof(iFilePath), dirName, _TRUNCATE);
            strncat_s(iFilePath, sizeof(iFilePath), fileName, _TRUNCATE);

            IFileMetadata metadata = decode_metadata(iFilePath);

            create_metadata_arr(&metadataArray, &metadataCount, &metadataCapacity, metadata);
        }
        free(rFilePaths);
        print_metadata(metadataArray, metadataCount);
    } else {
        printf("Recycle bin is empty!!\n");
    }
    // cleanup
    free(metadataArray);
}

void restore_file(const char *fileNameArg) {
    int count = 0;
    int fileFound = 0;
    char **rFilePaths = list_$R(&count);

    if (rFilePaths) {
        for (int i = 0; i < count; i++) {
            // get the $I from $R
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char iFilePath[MAX_PATH];
            const char *filename = PathFindFileName(rFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - rFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), rFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(iFilePath, sizeof(iFilePath), dirName, _TRUNCATE);
            strncat_s(iFilePath, sizeof(iFilePath), fileName, _TRUNCATE);
            // decode the $I
            IFileMetadata metadata = decode_metadata(iFilePath);
            if (strcmp(metadata.fileName, fileNameArg) == 0) {
                fileFound = 1;
                char fromPath[MAX_PATH + 2];
                char toPath[MAX_PATH + 2];

                // double null terminate
                strncpy_s(fromPath, sizeof(fromPath), rFilePaths[i], _TRUNCATE);
                fromPath[strlen(fromPath) + 1] = '\0';
                strncpy_s(toPath, sizeof(toPath), metadata.utf8Path, _TRUNCATE);
                toPath[sizeof(toPath) - 1] = '\0';

                // restore file using SHFileOperation
                SHFILEOPSTRUCT fileOp = {0};
                fileOp.wFunc = FO_MOVE;
                fileOp.pFrom = fromPath;
                fileOp.pTo = toPath;
                fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;

                if (SHFileOperation(&fileOp) != 0) {
                    fprintf(stderr, "Failed to restore: %s\n", metadata.fileName);
                }
                break;
            }  
        }
        free(rFilePaths);
    }

    if (!fileFound) {
        printf("'%s' doesn't exist in the recycle bin!!\n", fileNameArg);
    }
}

void restore_file_to_dest(const char *fileNameArg, const char *destPath) {
    int count = 0;
    int fileFound = 0;
    char **rFilePaths = list_$R(&count);

    if (rFilePaths) {
        for (int i = 0; i < count; i++) {
            // get the $I
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char iFilePath[MAX_PATH];
            const char *filename = PathFindFileName(rFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - rFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), rFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(iFilePath, sizeof(iFilePath), dirName, _TRUNCATE);
            strncat_s(iFilePath, sizeof(iFilePath), fileName, _TRUNCATE);
            // decode the $I
            IFileMetadata metadata = decode_metadata(iFilePath);
            if (strcmp(metadata.fileName, fileNameArg) == 0) {
                fileFound = 1;
                char toPath[MAX_PATH + 2];
                char fromPath[MAX_PATH + 2];

                // prepend a slash
                size_t fileNameLen = strlen(metadata.fileName);
                char fileNameWithSlash[fileNameLen + 2];
                fileNameWithSlash[0] = '/';
                strncpy_s(fileNameWithSlash + 1, sizeof(fileNameWithSlash) - 1, metadata.fileName, _TRUNCATE);

                // double null terminate
                strncpy_s(fromPath, sizeof(fromPath), rFilePaths[i], _TRUNCATE);
                fromPath[strlen(fromPath) + 1] = '\0';

                strncpy_s(toPath, sizeof(toPath), destPath, _TRUNCATE);
                strncat_s(toPath, sizeof(toPath), fileNameWithSlash, _TRUNCATE);
                toPath[sizeof(toPath) - 1] = '\0';
                // restore file using SHFileOperation
                SHFILEOPSTRUCT fileOp = {0};
                fileOp.wFunc = FO_MOVE;
                fileOp.pFrom = fromPath;
                fileOp.pTo = toPath;
                fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;

                if (SHFileOperation(&fileOp) != 0) {
                    fprintf(stderr, "Failed to restore: %s\n", metadata.fileName);
                }
                break;
            }
        }
        free(rFilePaths);
    }
    if (!fileFound) {
        printf("'%s' doesn't exist in the recycle bin!!\n", fileNameArg);
    }
}

void empty_recycle_bin() {
    if (!ask_confirmation()) {
        exit(EXIT_SUCCESS);
    }
    int count = 0;
    char **rFilePaths = list_$R(&count);
    if (rFilePaths) {
        for (int i = 0; i < count; i++) {
            // get the $I
            char dirName[MAX_PATH];
            char fileName[MAX_PATH];
            char iFilePath[MAX_PATH];
            const char *filename = PathFindFileName(rFilePaths[i]);
            strcpy_s(fileName, sizeof(fileName), filename);
            size_t dirLength = filename - rFilePaths[i];
            strncpy_s(dirName, sizeof(dirName), rFilePaths[i], dirLength);
            char *prefix = strstr(fileName, "$R");
            if (prefix != NULL) {
                prefix[1] = 'I';
            }
            strncpy_s(iFilePath, sizeof(iFilePath), dirName, _TRUNCATE);
            strncat_s(iFilePath, sizeof(iFilePath), fileName, _TRUNCATE);

            if (!DeleteFile(rFilePaths[i])) {
                perror("Failed to delete");
                /* fprintf(stderr, "Failed to delete file: %s\n", rFilePaths[i]); */
            }

            if (!DeleteFile(iFilePath)) {
                perror("Failed to delete");
                /* fprintf(stderr, "Failed to delete file: %s\n", rFilePaths[i]); */
            }
        }
        free(rFilePaths);
    } else {
        printf("Recycle bin is empty!!\n");
    }
}

int put_file(const char *filePath) {
    char *path = NULL;
    size_t pathLen = strlen(filePath);
    // if path exceeds the `MAX_PATH`
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
    // dynamic allocation
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
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NO_UI;
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
