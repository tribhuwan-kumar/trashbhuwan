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

#pragma pack(push, 1)
typedef struct {
    DWORD header;
    DWORD reserved;
    ULONGLONG fileSize;
    FILETIME deletionTime;
    WCHAR originalPath[];  // its null terminated
} IFILEENTRY;
#pragma pack(pop)

void decode_metadata(const char *iFilePath) {
    char readableSize[32];
    HANDLE hFile = CreateFileA(iFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open: %s\n", iFilePath);
        return;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    BYTE *buffer = (BYTE *)malloc(fileSize);
    DWORD bytesRead = 0;

    if (!ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
        printf("Failed to read: %s\n", iFilePath);
        CloseHandle(hFile);
        free(buffer);
        return;
    }

    SYSTEMTIME stUTC, stLocal;
    DWORD header = *(DWORD *)(buffer + 0);
    ULONGLONG size = *(ULONGLONG *)(buffer + 8);
    FILETIME deletionTime = *(FILETIME *)(buffer + 16);
    WCHAR *originalPath = (WCHAR *)(buffer + 28); // leave two empty bytes, correct offset

    char utf8Path[MAX_PATH * 4];
    WideCharToMultiByte(CP_UTF8, 0, originalPath, -1, utf8Path, sizeof(utf8Path), NULL, NULL);

    // ignore deletion time
    FileTimeToSystemTime(&deletionTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    readable_size(size, readableSize, sizeof(readableSize));
    char dirName[MAX_PATH];
    char fileName[MAX_PATH];

    const char *filename = PathFindFileName(utf8Path);
    strcpy(fileName, filename);
    size_t dirLength = filename - utf8Path;
    strncpy(dirName, utf8Path, dirLength);

    printf("%-10s %-10s %s\n", readableSize, dirName, fileName);

    // deletion time
    /* printf("Deleted on: %04d-%02d-%02d %02d:%02d:%02d\n", */
    /*        stLocal.wYear, stLocal.wMonth, stLocal.wDay, */
    /*        stLocal.wHour, stLocal.wMinute, stLocal.wSecond); */

    CloseHandle(hFile);
    free(buffer);
}

void readable_size(ULONGLONG size, char *buffer, size_t buffer_size) {
    const char *units[] = {"B", "KB", "MB", "GB"};
    double readableSize = (double)size;
    int unitIndex = 0;

    while (readableSize >= 1024 && unitIndex < 3) {
        readableSize /= 1024;
        unitIndex++;
    }

    if (unitIndex == 2) {
        snprintf(buffer, buffer_size, "%.1f %s", readableSize, units[unitIndex]);
    } else if (unitIndex == 3) {
        snprintf(buffer, buffer_size, "%.2f %s", readableSize, units[unitIndex]);
    } else {
        snprintf(buffer, buffer_size, "%.0f %s", readableSize, units[unitIndex]);
    }
}

#endif
