#include "xinput.h"

#include "../config.h"

#include <winternl.h>
#include <TlHelp32.h>
#include <SDKDDKVer.h>
#include <windows.h>
#include <Unknwn.h>

namespace injector
{

    void load()
    {
        char syspath[MAX_PATH];
        GetSystemDirectoryA(syspath, MAX_PATH);
        strcat_s(syspath, "\\xinput9_1_0.dll");
        hMod = LoadLibraryA(syspath);
        if (hMod > (HMODULE)31)
        {
            oXInputEnable = (tXInputEnable)GetProcAddress(hMod, "XInputEnable");
            oXInputGetState = (tXInputGetState)GetProcAddress(hMod, "XInputGetState");
            oXInputSetState = (tXInputSetState)GetProcAddress(hMod, "XInputSetState");
            oXInputGetKeystroke = (tXInputGetKeystroke)GetProcAddress(hMod, "XInputGetKeystroke");
            oXInputGetCapabilities = (tXInputGetCapabilities)GetProcAddress(hMod, "XInputGetCapabilities");
            oXInputGetDSoundAudioDeviceGuids = (tXInputGetDSoundAudioDeviceGuids)GetProcAddress(hMod, "XInputGetDSoundAudioDeviceGuids");
            oXInputGetBatteryInformation = (tXInputGetBatteryInformation)GetProcAddress(hMod, "XInputGetBatteryInformation");
            oXInputGetAudioDeviceIds = (tXInputGetAudioDeviceIds)GetProcAddress(hMod, "XInputGetAudioDeviceIds");
        }
    }

    void load_dlls()
    {
        std::filesystem::create_directory(MAIN_DIR "\\dll");

        for (auto &p : std::filesystem::directory_iterator(MAIN_DIR "\\dll"))
        {
            if (p.path().extension() == ".dll")
            {
                auto dll_name = p.path().filename().string();
                auto dll_path = MAIN_DIR "\\dll\\" + dll_name;

                auto hMod = LoadLibraryA(dll_path.c_str());
                if (hMod > (HMODULE)31)
                {
                    auto oDllMain = GetProcAddress(hMod, "DllMain");
                    if (oDllMain)
                    {
                        typedef BOOL(WINAPI * tDllMain)(HINSTANCE, DWORD, LPVOID);
                        tDllMain DllMain = (tDllMain)oDllMain;
                        DllMain(hMod, DLL_PROCESS_ATTACH, nullptr);
                    }
                }
            }
        }
    }
}