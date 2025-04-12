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

void put_file(const char *file_path) {
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

void delete_file(const char *file_path) {
    if (DeleteFile(file_path)) {
        printf("Deleted: %s\n", file_path);
    } else {
        printf("Failed to delete: %s\n", file_path);
    }
}



#endif
