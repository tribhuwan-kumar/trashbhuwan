#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <propidl.h>  // For PROPERTYKEY
#pragma comment(lib, "shell32.lib")

void list_recycle_bin_detailed() {
    // Initialize COM
    CoInitialize(nullptr);

    // Create shell object
    IShellDispatchPtr spShell;
    HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER,
                                 IID_IShellDispatch, (LPVOID*)&spShell);
    
    if (FAILED(hr)) {
        printf("Failed to create shell object\n");
        CoUninitialize();
        return;
    }

    // Get Recycle Bin folder
    FolderPtr spRecycleBin;
    hr = spShell->NameSpace(10 /*CSIDL_BITBUCKET*/, &spRecycleBin);
    if (FAILED(hr)) {
        printf("Failed to open Recycle Bin\n");
        spShell.Release();
        CoUninitialize();
        return;
    }

    // Get items in Recycle Bin
    FolderItemsPtr spItems;
    hr = spRecycleBin->Items(&spItems);
    if (FAILED(hr)) {
        printf("Failed to get Recycle Bin items\n");
        spRecycleBin.Release();
        spShell.Release();
        CoUninitialize();
        return;
    }

    // Print header
    printf("Files in Recycle Bin:\n");
    printf("%-50s %-20s\n", "Original Path", "Size");
    printf("----------------------------------------\n");

    // Iterate through items
    long itemCount = spItems->Count;
    for (long i = 0; i < itemCount; i++) {
        FolderItemPtr pItem;
        hr = spItems->Item(i, &pItem);
        if (SUCCEEDED(hr)) {
            // Get original path
            BSTR bstrPath;
            hr = pItem->GetExtendedProperty("System.Recycle.DeletedFrom", &bstrPath);
            if (SUCCEEDED(hr)) {
                char szPath[MAX_PATH];
                WideCharToMultiByte(CP_ACP, 0, bstrPath, -1, 
                                  szPath, MAX_PATH, NULL, NULL);
                SysFreeString(bstrPath);
                
                // Get size
                VARIANT vtSize;
                VariantInit(&vtSize);
                hr = pItem->get_Size(&vtSize);
                if (SUCCEEDED(hr)) {
                    printf("%-50s %-20I64d bytes\n", 
                           szPath, vtSize.uhVal.ui64);
                    VariantClear(&vtSize);
                }
            }
            pItem.Release();
        }
    }

    // Cleanup
    spItems.Release();
    spRecycleBin.Release();
    spShell.Release();
    CoUninitialize();
}

int main() {
    list_recycle_bin_detailed();
    return 0;
}
