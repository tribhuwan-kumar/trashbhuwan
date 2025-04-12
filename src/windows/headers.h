#ifdef _WIN32
#ifndef HEADERS_H
#define HEADERS_H
#include <windows.h>

void list_recycle_bin_items();
void move_to_trash(const char *file_path);
void restore_file(const char *file_path);
void delete_file(const char *file_path);
void readable_size(ULONGLONG size, char *buffer, size_t buffer_size);
void decode_metadata(const char *iFilePath);

#endif
#endif
