#ifdef _WIN32
#ifndef HEADERS_H
#define HEADERS_H
#include <windows.h>
#include <shlwapi.h>

typedef struct {
    char utf8Path[MAX_PATH * 4];
    char readableSize[32];
    char fileName[MAX_PATH];
    char dirName[MAX_PATH];
} IFileMetadata;

void list_recycle_bin();
void empty_recycle_bin();
char **list_$R(int *count);
int put_file(const char *file_path);
void delete_file(const char *file_path);
void restore_file(const char *fileName);
IFileMetadata decode_metadata(const char *iFilePath);
void readable_size(ULONGLONG size, char *buffer, size_t bufferSize);
void print_metadata(IFileMetadata *metadataArray, int metadataCount);
void restore_file_to_dest(const char *fileNameArg, const char *destPath);
void create_metadata_arr(IFileMetadata **metadataArray, int *metadataCount, int *metadataCapacity, IFileMetadata newMetadata);

#endif
#endif
