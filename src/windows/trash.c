#ifdef _WIN32
#include <stdio.h>
#include <stdint.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windows.h>
#include <stdlib.h>
#include <wchar.h>

#include "./headers.h"

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
        CoTaskMemFree((LPVOID)pidlRecycleBin);
        CoUninitialize();
        return;
    }

    // enumerate items in the Recycle Bin
    hr = pRecycleBinFolder->lpVtbl->EnumObjects(pRecycleBinFolder, NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumIDList);
    if (FAILED(hr)) {
        printf("Failed to enumerate items in Recycle Bin.\n");
        pRecycleBinFolder->lpVtbl->Release(pRecycleBinFolder);
        CoTaskMemFree((LPVOID)pidlRecycleBin);
        CoUninitialize();
        return;
    }

    LPITEMIDLIST pidlItem;
    ULONG fetched;
    while (pEnumIDList->lpVtbl->Next(pEnumIDList, 1, &pidlItem, &fetched) == S_OK) {
        STRRET str;
        char filePath[MAX_PATH];
        WIN32_FIND_DATA findData;

        // get the display name of the item
        hr = pRecycleBinFolder->lpVtbl->GetDisplayNameOf(pRecycleBinFolder, pidlItem, SHGDN_FORPARSING, &str);
        if (SUCCEEDED(hr)) {
            // convert STRRET to a string
            StrRetToBuf(&str, pidlItem, filePath, MAX_PATH);

            // get file metadata
            if (SHGetDataFromIDList(pRecycleBinFolder, pidlItem, SHGDFIL_FINDDATA, &findData, sizeof(findData)) == S_OK) {
                char dirName[MAX_PATH];
                char fileName[MAX_PATH];

                const char *filename = PathFindFileName(filePath);
                strcpy(fileName, filename);
                size_t dirLength = filename - filePath;
                strncpy(dirName, filePath, dirLength);

                char *prefix = strstr(fileName, "$R");
                if (prefix != NULL) {
                    prefix[1] = 'I';
                }
                snprintf(filePath, sizeof(filePath), "%s%s", dirName, fileName);
                decode_metadata(filePath);
            }
        }

        CoTaskMemFree((LPVOID)pidlItem);
    }

    // cleanup
    pEnumIDList->lpVtbl->Release(pEnumIDList);
    pRecycleBinFolder->lpVtbl->Release(pRecycleBinFolder);
    CoTaskMemFree((LPVOID)pidlRecycleBin);
    CoUninitialize();
}

void restore_file(const char *file_path) {
    // Restoring files from the Recycle Bin is more complex and requires
    // interacting with the shell namespace. This is a simplified example.
    printf("Restoring files from the Recycle Bin is not implemented in this example.\n");
}

void restore_file_to_dest(const char *file_path) {
    // Restoring files from the Recycle Bin is more complex and requires
    // interacting with the shell namespace. This is a simplified example.
    printf("Restoring files from the Recycle Bin is not implemented in this example.\n");
}

void empty_trash(const char *file_path) {
    /* empty_trash trash */
}

int put_file(const char *file_path) {
    char *path = NULL;
    size_t path_len = strlen(file_path);

    if (path_len >= MAX_PATH) {
        path = (char *)malloc(path_len + 5); // null terminator included
        if (!path) {
            fprintf(stderr, "Memory allocation failed!!\n");
            return 1;
        }
        snprintf(path, path_len + 5, "\\\\?\\%s", file_path);
    } else {
        static char local_path[MAX_PATH + 1];
        path = local_path;
        snprintf(path, MAX_PATH + 1, "%s", file_path);
    }
    
    size_t path_with_null_len = strlen(path) + 2;
    char *path_with_null = (char *)malloc(path_with_null_len);

    if (!path_with_null) {
        fprintf(stderr, "Memory allocation failed!!\n");
        if (path != file_path) free(path);
        return 1;
    }
    snprintf(path_with_null, path_with_null_len, "%s", path);
    path_with_null[path_with_null_len - 1] = '\0'; // double null-terminate

    // prepare SHFILEOPSTRUCT
    SHFILEOPSTRUCT file_op = {0};
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_with_null;
    file_op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

    if (SHFileOperation(&file_op) == 0) {
        return 0; 
    } else {
        fprintf(stderr, "Failed to put file in recycle bin!!\n");
        return 1; 
    }

    // cleanup
    if (path != file_path) free(path);
    free(path_with_null);
}

void delete_file(const char *file_path) {
    if (DeleteFile(file_path)) {
        printf("Deleted: %s\n", file_path);
    } else {
        printf("Failed to delete: %s\n", file_path);
    }
}



#endif
