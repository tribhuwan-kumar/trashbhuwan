#ifndef UTILS_H
#define UTILS_H
#define YELLOW "\033[0;33m"
#define NC "\033[0m"

extern const char *__size_char;
extern const char *__deleted_from_char;
extern const char *__files_dirs_char;
extern const char *__total_size_char;
extern const char *__dash_char;

int ask_confirmation();
void formatting(const char *fileSize, const char *paths, const char *fileName, int sizeWidth, int dirWidth);

#endif
