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
