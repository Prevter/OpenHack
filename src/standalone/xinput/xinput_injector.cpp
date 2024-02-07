#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "xinput.hpp"

tXInputEnable oXInputEnable = nullptr;
tXInputGetState oXInputGetState = nullptr;
tXInputSetState oXInputSetState = nullptr;
tXInputGetKeystroke oXInputGetKeystroke = nullptr;
tXInputGetCapabilities oXInputGetCapabilities = nullptr;
tXInputGetDSoundAudioDeviceGuids oXInputGetDSoundAudioDeviceGuids = nullptr;
tXInputGetBatteryInformation oXInputGetBatteryInformation = nullptr;
tXInputGetAudioDeviceIds oXInputGetAudioDeviceIds = nullptr;
HMODULE hMod = nullptr;

void WINAPI XInputEnable(WINBOOL x1)
{
    if (oXInputEnable)
        oXInputEnable(x1);
};
DWORD WINAPI XInputSetState(DWORD x1, XINPUT_VIBRATION *x2)
{
    if (oXInputSetState)
        return oXInputSetState(x1, x2);
    return ERROR_DEVICE_NOT_CONNECTED;
};
DWORD WINAPI XInputGetState(DWORD x1, XINPUT_STATE *x2)
{
    if (oXInputGetState)
        return oXInputGetState(x1, x2);
    return ERROR_DEVICE_NOT_CONNECTED;
};
DWORD WINAPI XInputGetKeystroke(DWORD x1, DWORD x2, PXINPUT_KEYSTROKE x3)
{
    if (oXInputGetKeystroke)
        return oXInputGetKeystroke(x1, x2, x3);
    return ERROR_EMPTY;
};
DWORD WINAPI XInputGetCapabilities(DWORD x1, DWORD x2, XINPUT_CAPABILITIES *x3)
{
    if (oXInputGetCapabilities)
        return oXInputGetCapabilities(x1, x2, x3);
    return ERROR_DEVICE_NOT_CONNECTED;
};
DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD x1, GUID *x2, GUID *x3)
{
    if (oXInputGetDSoundAudioDeviceGuids)
        return oXInputGetDSoundAudioDeviceGuids(x1, x2, x3);
    return ERROR_DEVICE_NOT_CONNECTED;
};
DWORD WINAPI XInputGetBatteryInformation(DWORD x1, BYTE x2, XINPUT_BATTERY_INFORMATION *x3)
{
    if (oXInputGetBatteryInformation)
        return oXInputGetBatteryInformation(x1, x2, x3);
    return ERROR_EMPTY;
};

DWORD WINAPI XInputGetAudioDeviceIds(DWORD x1, LPWSTR x2, UINT *x3, LPWSTR x4, UINT *x5)
{
    if (oXInputGetAudioDeviceIds)
        return oXInputGetAudioDeviceIds(x1, x2, x3, x4, x5);
    return ERROR_DEVICE_NOT_CONNECTED;
}