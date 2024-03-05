#include "pch.hpp"

#include "hooks/hooks.hpp"
#include "debug/crashhandler.hpp"
#include "imgui/imgui_hook.hpp"
#include "utils.hpp"
#include "updater/updater.hpp"

#ifdef PLATFORM_WINDOWS

#include "xinput/injector.hpp"

bool firstInit = true;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            // Setup XInput bindings
            xinput::initialize();

            // Debugging
            logger::initialize(false, true, "openhack.log");
            crashhandler::init();

            // TODO: Add support for other versions
            if (openhack::utils::getGameVersion() != "2.204") {
                L_ERROR("Unsupported game version");
                return FALSE;
            }

            // Initialize OpenHack
            openhack::initialize();

            // Setup ImGui
            ImGuiHook::setInitCallback([]() {
                if (firstInit) {
                    firstInit = false;

                    // Check for updates
                    if (config::get<bool>("menu.checkForUpdates", true)) {
                        openhack::updater::check([](bool available, const openhack::updater::Version& version) {
                            if (!available) return;
                            if (version.version == OPENHACK_VERSION) return;
                            if (version.downloadUrl.empty()) return;

                            openhack::config::setGlobal("update.available", true);
                            openhack::config::setGlobal("update.version", version.version);
                            openhack::config::setGlobal("update.downloadUrl", version.downloadUrl);
                            openhack::config::setGlobal("update.title", version.title);
                            openhack::config::setGlobal("update.changelog", version.changelog);
                        });
                    }
                }

                openhack::menu::init();
            });
            ImGuiHook::setDrawCallback(openhack::menu::draw);

            // Install hooks
            openhack::hooks::installHooks();

            // Load DLL files from the "dll" directory
            auto dllDir = openhack::utils::getModAssetsDirectory() / "dll";

            // Make sure the directory exists
            if (!std::filesystem::exists(dllDir)) {
                std::filesystem::create_directory(dllDir);
            }

            // Inject all DLL files in the directory
            for (const auto &entry: std::filesystem::directory_iterator(dllDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".dll") {
                    openhack::win32::injectDll(entry.path().string());
                }
            }

            L_TRACE("Initialization completed");
            break;
        }
        case DLL_PROCESS_DETACH:
            ImGuiHook::destroy();
            openhack::deinitialize();
            break;

        default:
            break;
    }
    return TRUE;
}

#endif