#pragma once

#include <cstdint>
#include <fstream>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>

namespace openhack::win32 {

    /// @brief Inject a DLL into current process.
    /// @param dllPath The path to the DLL to inject.
    /// @return Whether the injection was successful.
    inline bool injectDll(const std::string &dllPath) {
        // Get module handle
        HMODULE hModule = LoadLibraryA(dllPath.c_str());

        // Check if the module handle is valid
        if (hModule == nullptr) {
            return false;
        }

        // Call DllMain with DLL_PROCESS_ATTACH
        if (hModule > (HMODULE) 31) {
            auto oDllMain = (BOOL(WINAPI *)(HMODULE, DWORD, LPVOID)) GetProcAddress(hModule, "DllMain");
            if (oDllMain != nullptr) {
                oDllMain(hModule, DLL_PROCESS_ATTACH, nullptr);
            }
        }

        return true;
    }

    /// @brief Open a file dialog to select a DLL to inject. Injects the DLL into the current process.
    inline void promptDllInjection() {
        // Open a file dialog to select a DLL to inject
        OPENFILENAME ofn;
        char szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Dynamic Link Libraries\0*.dll\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        GetOpenFileName(&ofn);

        // Inject the DLL into the current process
        injectDll(szFile);
    }
}