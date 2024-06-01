#include "xinput.hpp"

#include <winternl.h>
#include <TlHelp32.h>
#include <SDKDDKVer.h>
#include <windows.h>
#include <Unknwn.h>

/// @brief Proxy-loader for the XInput library.
namespace xinput {
    /// @brief Loads the XInput library and initializes the function pointers.
    void initialize() {
        char syspath[MAX_PATH];
        GetSystemDirectoryA(syspath, MAX_PATH);
        strcat_s(syspath, "\\xinput1_4.dll");
        hMod = LoadLibraryA(syspath);
        if (hMod > (HMODULE) 31) {
            oXInputEnable = (tXInputEnable) GetProcAddress(hMod, "XInputEnable");
            oXInputGetState = (tXInputGetState) GetProcAddress(hMod, "XInputGetState");
            oXInputSetState = (tXInputSetState) GetProcAddress(hMod, "XInputSetState");
            oXInputGetKeystroke = (tXInputGetKeystroke) GetProcAddress(hMod, "XInputGetKeystroke");
            oXInputGetCapabilities = (tXInputGetCapabilities) GetProcAddress(hMod, "XInputGetCapabilities");
            oXInputGetDSoundAudioDeviceGuids = (tXInputGetDSoundAudioDeviceGuids) GetProcAddress(hMod,
                                                                                                 "XInputGetDSoundAudioDeviceGuids");
            oXInputGetBatteryInformation = (tXInputGetBatteryInformation) GetProcAddress(hMod,
                                                                                         "XInputGetBatteryInformation");
            oXInputGetAudioDeviceIds = (tXInputGetAudioDeviceIds) GetProcAddress(hMod, "XInputGetAudioDeviceIds");
        }
    }
}