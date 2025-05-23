#ifdef _WIN32
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include <wchar.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "./headers.h"
#include "../common/utils.h"

#pragma pack(push, 1)
typedef struct {
    DWORD header;
    DWORD reserved;
    ULONGLONG fileSize;
    FILETIME deletionTime;
    WCHAR originalPath[];  // its null terminated
} IFILEENTRY;
#pragma pack(pop)

// the $I files are in little endian format
IFileMetadata decode_metadata(const char *iFilePath) {
    IFileMetadata metadata = {0};
    HANDLE hFile = CreateFileA(iFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open: %s\n", iFilePath);
        return metadata;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    BYTE *buffer = (BYTE *)malloc(fileSize);
    DWORD bytesRead = 0;

    if (!ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
        printf("Failed to read: %s\n", iFilePath);
        CloseHandle(hFile);
        free(buffer);
        return metadata;
    }

    DWORD header = *(DWORD *)(buffer + 0);
    ULONGLONG size = *(ULONGLONG *)(buffer + 8);
    FILETIME deletionTime = *(FILETIME *)(buffer + 16);
    WCHAR *originalPath = (WCHAR *)(buffer + 28); // leave two empty bytes, correct offset

    // ignore deletion time
    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&deletionTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    WideCharToMultiByte(CP_UTF8, 0, originalPath, -1, metadata.utf8Path, sizeof(metadata.utf8Path), NULL, NULL);
    readable_size(size, metadata.readableSize, sizeof(metadata.readableSize));

    const char *filename = PathFindFileName(metadata.utf8Path);
    strcpy_s(metadata.fileName, sizeof(metadata.fileName), filename);

    size_t dirLength = filename - metadata.utf8Path;
    strncpy_s(metadata.dirName, sizeof(metadata.dirName), metadata.utf8Path, dirLength);

    CloseHandle(hFile);
    free(buffer);
    return metadata;
}

void create_metadata_arr(IFileMetadata **metadataArray, int *metadataCount, int *metadataCapacity, IFileMetadata newMetadata) {
    if (*metadataCount >= *metadataCapacity) {
        *metadataCapacity *= 2;
        *metadataArray = (IFileMetadata *)realloc(*metadataArray, *metadataCapacity * sizeof(IFileMetadata));
        if (!*metadataArray) {
            fprintf(stderr, "Error: Memory reallocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    (*metadataArray)[*metadataCount] = newMetadata;
    (*metadataCount)++;
}

void readable_size(ULONGLONG size, char *buffer, size_t bufferSize) {
    const char *units[] = {"B", "K", "M", "G"};
    double readableSize = (double)size;
    int unitIndex = 0;

    while (readableSize >= 1024 && unitIndex < 3) {
        readableSize /= 1024;
        unitIndex++;
    }

    if (unitIndex == 2) {
        snprintf(buffer, bufferSize, "%.1f%s", readableSize, units[unitIndex]);
    } else if (unitIndex == 3) {
        snprintf(buffer, bufferSize, "%.2f%s", readableSize, units[unitIndex]);
    } else {
        snprintf(buffer, bufferSize, "%.0f%s", readableSize, units[unitIndex]);
    }
}

ULONGLONG parse_readable_size(const char *readableSize) {
    ULONGLONG size = 0;
    double value;
    char unit[3] = {0};

    if (sscanf_s(readableSize, "%lf%2s", &value, unit, (unsigned int)sizeof(unit)) == 2) {
        if (strcmp(unit, "K") == 0) {
            size = (ULONGLONG)(value * 1024);
        } else if (strcmp(unit, "M") == 0) {
            size = (ULONGLONG)(value * 1024 * 1024);
        } else if (strcmp(unit, "G") == 0) {
            size = (ULONGLONG)(value * 1024 * 1024 * 1024);
        } else {
            size = (ULONGLONG)value;
        }
    }
    return size;
}

ULONGLONG calculate_total_size(IFileMetadata *metadataArray, int metadataCount) {
    ULONGLONG totalSize = 0;
    for (int i = 0; i < metadataCount; i++) {
        totalSize += parse_readable_size(metadataArray[i].readableSize);
    }
    return totalSize;
}

void print_metadata(IFileMetadata *metadataArray, int metadataCount) {
    int sizeWidth = 8;
    int maxDirWidth = strlen(__files_dirs_char);

    for (int i = 0; i < metadataCount; i++) {
        int pathLen = strlen(metadataArray[i].dirName);

        if (pathLen > maxDirWidth) maxDirWidth = pathLen;
    }

    printf("%s%-*s%s%s%-*s%s%s%s%s\n", 
           YELLOW, sizeWidth+1, __size_char, NC,
           YELLOW, maxDirWidth+5, __deleted_from_char, NC,
           YELLOW, __files_dirs_char, NC
           );

    for (int i = 0; i < metadataCount; i++) {
        printf("%-*s %-*s     %s\n", 
               sizeWidth, metadataArray[i].readableSize,
               maxDirWidth, metadataArray[i].dirName,
               metadataArray[i].fileName
            );
    }

    ULONGLONG totalSize = calculate_total_size(metadataArray, metadataCount);
    char totalSizeReadable[32];
    readable_size(totalSize, totalSizeReadable, sizeof(totalSizeReadable));

    printf("%s%-*s%s%s%-*s%s%s%s%s\n", 
           YELLOW, sizeWidth+1, totalSizeReadable, NC,
           YELLOW, maxDirWidth+5, __total_size_char, NC,
           YELLOW, __dash_char, NC
        );
}

char **list_$R(int *count) {
    char **filePaths = NULL;
    int fileCount = 0;

    HRESULT hr;
    LPITEMIDLIST pidlRecycleBin;
    IShellFolder *pRecycleBinFolder = NULL;
    IEnumIDList *pEnumIDList = NULL;

    // initialize COM
    CoInitialize(NULL);

    // get the Recycle Bin directory
    hr = SHGetSpecialFolderLocation(NULL, CSIDL_BITBUCKET, &pidlRecycleBin);
    if (FAILED(hr)) {
        printf("Failed to get Recycle Bin folder.\n");
        CoUninitialize();
        return NULL;
    }

    // get IShellFolder interface
    hr = SHBindToObject(NULL, pidlRecycleBin, NULL, &IID_IShellFolder, (void**)&pRecycleBinFolder);
    if (FAILED(hr)) {
        printf("Failed to bind to Recycle Bin folder.\n");
        CoTaskMemFree((LPVOID)pidlRecycleBin);
        CoUninitialize();
        return NULL;
    }

    // enumerate items in the Recycle Bin
    hr = pRecycleBinFolder->lpVtbl->EnumObjects(pRecycleBinFolder, NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumIDList);
    if (FAILED(hr)) {
        printf("Failed to enumerate items in Recycle Bin.\n");
        pRecycleBinFolder->lpVtbl->Release(pRecycleBinFolder);
        CoTaskMemFree((LPVOID)pidlRecycleBin);
        CoUninitialize();
        return NULL;
    }

    LPITEMIDLIST pidlItem;
    ULONG fetched;
    while (pEnumIDList->lpVtbl->Next(pEnumIDList, 1, &pidlItem, &fetched) == S_OK) {
        STRRET str;
        char filePath[MAX_PATH];
        WIN32_FIND_DATA findData;

        // get the items, it'll be a $R file
        hr = pRecycleBinFolder->lpVtbl->GetDisplayNameOf(pRecycleBinFolder, pidlItem, SHGDN_FORPARSING, &str);
        if (SUCCEEDED(hr)) {
            // convert STRRET to a string
            StrRetToBuf(&str, pidlItem, filePath, MAX_PATH);
            // fetch some metadata data too
            if (SHGetDataFromIDList(pRecycleBinFolder, pidlItem, SHGDFIL_FINDDATA, &findData, sizeof(findData)) == S_OK) {
                filePaths = realloc(filePaths, (fileCount + 1) * sizeof(char *));
                filePaths[fileCount] = _strdup(filePath);
                fileCount++;
            }
        }
        CoTaskMemFree((LPVOID)pidlItem);
    }
    // cleanup
    pEnumIDList->lpVtbl->Release(pEnumIDList);
    pRecycleBinFolder->lpVtbl->Release(pRecycleBinFolder);
    CoTaskMemFree((LPVOID)pidlRecycleBin);
    CoUninitialize();

    *count = fileCount;
    return filePaths;
}

#endif
