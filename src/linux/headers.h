#ifndef HEADERS_H
#define HEADERS_H

const char* get_user_name();
char* get_absolute_path(const char* relPath);
char* get_path(const char* base, const char* suffix);
int is_directory_empty(const char *path);
int get_max_dir_width(const char *trashFilesDir, const char *trashInfoDir);
void restore_trashed_file(const char *fileName);
void delete_trashed_file(const char *fileName);
void restore_trashed_file_on_dest(const char *fileName, const char *destPath);
void create_trash_info(const char *filePath, const char *trashInfoDir);
void list_trashed_files(const char *trashFilesDir, const char *trashInfoDir);
void empty_trash(const char *trash_files_dir, const char *trash_info_dir);

#endif
