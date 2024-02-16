#include "pch.hpp"

#include "hooks/hooks.hpp"
#include "debug/crashhandler.hpp"
#include "imgui/imgui_hook.hpp"
#include "utils.hpp"

#ifdef PLATFORM_WINDOWS

#include "xinput/injector.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            // Setup XInput bindings
            xinput::initialize();

            // Debugging
            logger::initialize(true, true, "openhack.log");
            crashhandler::init();

            // Initialize OpenHack
            openhack::initialize();

            // Setup ImGui
            ImGuiHook::setInitCallback(openhack::menu::init);
            ImGuiHook::setDrawCallback(openhack::menu::draw);

            // Install hooks
            openhack::hooks::installHooks();
            break;

            // Load DLL files from the "dll" directory
            auto dllDir = openhack::utils::getModAssetsDirectory() + "/dll";
            for (const auto &entry: std::filesystem::directory_iterator(dllDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".dll") {
                    openhack::win32::injectDll(entry.path().string());
                }
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            ImGuiHook::destroy();
            openhack::deinitialize();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

#endif