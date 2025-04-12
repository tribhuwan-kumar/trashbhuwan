#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>
#include <stdio.h>

#define YELLOW "\033[0;33m"
#define NC "\033[0m"

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

    // convert deletion time to human-readable format
    FileTimeToSystemTime(&deletionTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    printf("Original Path: %s\n", utf8Path);
    printf("Size: %llu bytes\n", size);
    printf("Deleted on: %04d-%02d-%02d %02d:%02d:%02d\n",
           stLocal.wYear, stLocal.wMonth, stLocal.wDay,
           stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

    CloseHandle(hFile);
    free(buffer);
}

void list_recycle_bin_items() {
    HRESULT hr;
    LPITEMIDLIST pidlRecycleBin;
    IShellFolder *pRecycleBinFolder = NULL;
    IEnumIDList *pEnumIDList = NULL;

    // initialize COM
    CoInitialize(NULL);

    // get the Recycle Bin folder
    hr = SHGetSpecialFolderLocation(NULL, CSIDL_BITBUCKET, &pidlRecycleBin);
    if (FAILED(hr)) {
        printf("Failed to get Recycle Bin folder.\n");
        CoUninitialize();
        return;
    }

    // get IShellFolder interface for the Recycle Bin
    hr = SHBindToObject(NULL, pidlRecycleBin, NULL, &IID_IShellFolder, (void**)&pRecycleBinFolder);
    if (FAILED(hr)) {
        printf("Failed to bind to Recycle Bin folder.\n");
        CoTaskMemFree(pidlRecycleBin);
        CoUninitialize();
        return;
    }

    // enumerate items in the Recycle Bin
    hr = pRecycleBinFolder->lpVtbl->EnumObjects(pRecycleBinFolder, NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumIDList);
    if (FAILED(hr)) {
        printf("Failed to enumerate items in Recycle Bin.\n");
        pRecycleBinFolder->lpVtbl->Release(pRecycleBinFolder);
        CoTaskMemFree(pidlRecycleBin);
        CoUninitialize();
        return;
    }

    LPITEMIDLIST pidlItem;
    ULONG fetched;
    while (pEnumIDList->lpVtbl->Next(pEnumIDList, 1, &pidlItem, &fetched) == S_OK) {
        STRRET str;
        char fileName[MAX_PATH];
        WIN32_FIND_DATA findData;

        // get the display name of the item
        hr = pRecycleBinFolder->lpVtbl->GetDisplayNameOf(pRecycleBinFolder, pidlItem, SHGDN_FORPARSING, &str);
        if (SUCCEEDED(hr)) {
            // convert STRRET to a string
            StrRetToBuf(&str, pidlItem, fileName, MAX_PATH);

            // get file metadata
            if (SHGetDataFromIDList(pRecycleBinFolder, pidlItem, SHGDFIL_FINDDATA, &findData, sizeof(findData)) == S_OK) {
                // print in this format SIZE in yellow color 
                printf("File: %s\n", fileName);
                printf("Size: %llu bytes\n", ((ULONGLONG)findData.nFileSizeHigh << 32) | findData.nFileSizeLow);
            }
        }

        CoTaskMemFree(pidlItem);
    }

    // cleanup
    pEnumIDList->lpVtbl->Release(pEnumIDList);
    pRecycleBinFolder->lpVtbl->Release(pRecycleBinFolder);
    CoTaskMemFree(pidlRecycleBin);
    CoUninitialize();
}

/* int is_trash_empty() { */
/*     WCHAR recycleBinPath[MAX_PATH]; */
/*     WIN32_FIND_DATA findFileData; */
/*     HANDLE hFind; */
/**/
/*     // Get the path to the Recycle Bin */
/*     if (!SHGetSpecialFolderPathW(NULL, recycleBinPath, CSIDL_BITBUCKET, FALSE)) { */
/*         printf("Failed to get Recycle Bin path.\n"); */
/*         return -1; // Error */
/*     } */
/**/
/*     // Append wildcard to search for files in the Recycle Bin */
/*     wcscat(recycleBinPath, L"\\*"); */
/**/
/*     // Find the first file in the Recycle Bin */
/*     hFind = FindFirstFileW(recycleBinPath, &findFileData); */
/*     if (hFind == INVALID_HANDLE_VALUE) { */
/*         printf("Failed to enumerate Recycle Bin.\n"); */
/*         return -1; // Error */
/*     } */
/**/
/*     int isEmpty = 1; // Assume empty */
/*     do { */
/*         // Skip "." and ".." entries */
/*         if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) { */
/*             isEmpty = 0; // Found a file or folder */
/*             break; */
/*         } */
/*     } while (FindNextFileW(hFind, &findFileData)); */
/**/
/*     FindClose(hFind); */
/*     return isEmpty; */
/* } */


void delete_file(const char *file_path) {
    if (DeleteFile(file_path)) {
        printf("Deleted: %s\n", file_path);
    } else {
        printf("Failed to delete: %s\n", file_path);
    }
}

void move_to_trash(const char *file_path) {
    SHFILEOPSTRUCT file_op = {
        NULL,
        FO_DELETE,
        file_path,
        "",
        FOF_ALLOWUNDO | FOF_NOCONFIRMATION,
        FALSE,
        NULL,
        NULL
    };

    if (SHFileOperation(&file_op) == 0) {
        printf("Moved to Recycle Bin: %s\n", file_path);
    } else {
        printf("Failed to move to Recycle Bin: %s\n", file_path);
    }
}

void restore_file(const char *file_path) {
    // Restoring files from the Recycle Bin is more complex and requires
    // interacting with the shell namespace. This is a simplified example.
    printf("Restoring files from the Recycle Bin is not implemented in this example.\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        char* printAsciiCommand = "curl -s https://raw.githubusercontent.com/tribhuwan-kumar/trashbhuwan/main/tests/ascii";
        size_t printAsciiCommandSize = strlen(printAsciiCommand) + 1;
        char* printAscii = (char*)malloc(printAsciiCommandSize);
        snprintf(printAscii, printAsciiCommandSize, "%s", printAsciiCommand);
        if (system(printAsciiCommand) != 0) {
            free(printAscii);
        } else {
            free(printAscii);
        }
    } else if (argc == 2 && (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-ls") == 0 )) {
        /* list_recycle_bin_items(); */
        /* is_trash_empty(); */

        /* decode_metadata("C:\\$Recycle.Bin\\S-1-5-21-2594502966-2529061830-489477826-1003\\$IUG9CCZ.png"); */

        decode_metadata("C:\\$Recycle.Bin\\S-1-5-21-2594502966-2529061830-489477826-1003\\$ICH427R");
    } else if (strcmp(argv[1], "-h") == 0) {                                                                        // help
        printf("Usage: trashbhuwan --put [FILE] [DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
    } else if (strcmp(argv[1], "--help") == 0) {
        printf("Usage: trashbhuwan [OPTION] [FILE]...\n"
               "Trashing & Restoring files\n\n"
               "Usage: trashbhuwan --put [FILE] [DIRECTORY]\n"
               "Puts file and directory in trash\n\n"
               "Available flags:\n"
               "    -p,  --put              Put files & directories in trash\n"
               "    -r,  --restore          Restore files & directories from trash\n"
               "    -rd, --restore-dest     Restore files & directories to specific destination\n"
               "    -ls, --list             List trashed files & directories along with disk usage\n"
               "    -dl, --delete           Individually Delete trashed files & directories\n"
               "    -em, --empty            Empty the trash\n"
               "\nReport bugs to: <https://github.com/tribhuwan-kumar/trashbhuwan/issues>\n"
               "'trashbhuwan' home page: <https://github.com/tribhuwan-kumar/trashbhuwan>\n"
               "General more help: <https://github.com/tribhuwan-kumar/trashbhuwan#introduction>\n");
    } else {
        printf("Invalid option: %s\n", argv[1]);
        printf("Usage: [--list|-ls] [--put|-p] [--restore|-r] [--delete|-dl] [--empty|-em] [FILE]...[DIRECTORY]\n"
               "Try 'trashbhuwan --help' for more information\n");
    }

    return 0;
}
