#include "pch.hpp"

#include "hooks/hooks.hpp"
#include "debug/crashhandler.hpp"

#ifdef PLATFORM_WINDOWS
#include "xinput/injector.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        xinput::initialize();
        logger::initialize(true, true, "openhack.log");
        crashhandler::init();
        openhack::initialize();
        openhack::hooks::installHooks();
        break;
    case DLL_PROCESS_DETACH:
        openhack::deinitialize();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
#endif