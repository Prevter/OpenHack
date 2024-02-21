#include "shortcuts.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

#ifdef PLATFORM_WINDOWS

    void Shortcuts::patchGame() {
        bool success = win32::four_gb::patch();
        L_INFO("Patched the game to use 4GB of memory: {}", success);
        MessageBox(nullptr, success ? "Patched the game to use 4GB of memory. Please restart the game."
                                    : "Failed to patch the game. Could not write to the file.",
                   "4GB Patch", (success ? MB_ICONINFORMATION : MB_ICONERROR) | MB_OK);

        if (success) {
            // Close the game
            std::exit(0);
        }
    }

#else

    void Shortcuts::patchGame() {
        L_WARN("4GB patch is not supported on this platform.");
    }

#endif

    void Shortcuts::openOptions() {
    }

    void Shortcuts::restartLevel() {
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer) {
            playLayer->resetLevel();
        }
    }

    void Shortcuts::togglePractice() {
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer) {
            bool isPractice = playLayer->m_isPracticeMode();
            playLayer->togglePracticeMode(!isPractice);
        }
    }

    void Shortcuts::openResources() {
        auto path = utils::getCurrentDirectory() + "/Resources";
        utils::openDirectory(path.c_str());
    }

    void Shortcuts::openAppData() {
        auto path = utils::getSaveDirectory();
        utils::openDirectory(path.c_str());
    }

    void Shortcuts::onInit() {
        // Create window
        menu::addWindow("Shortcuts", [&]() {

            if (gui::button("Show Options"))
                openOptions();
            gui::tooltip("Opens the game's options menu.");
            menu::keybinds::addMenuKeybind("shortcuts.options", "Show Options", []() {
                openOptions();
            });

            if (gui::button("Restart Level"))
                restartLevel();
            gui::tooltip("Restarts the current level.");
            menu::keybinds::addMenuKeybind("shortcuts.restart_level", "Restart Level", []() {
                restartLevel();
            });

            if (gui::button("Practice Mode"))
                togglePractice();
            gui::tooltip("Toggles practice mode.");
            menu::keybinds::addMenuKeybind("shortcuts.practice_mode", "Practice Mode", []() {
                togglePractice();
            });

#ifdef PLATFORM_WINDOWS
                if (!win32::four_gb::isPatched()) {
                    if (gui::button("Apply 4GB patch")) {
                        patchGame();
                    }
                    gui::tooltip("Highly recommended to install.\n"
                                 "Allows the game to use more memory, which resolves some crashes.");
                }
            if (gui::button("Inject DLL")) {
                win32::promptDllInjection();
            }
#ifdef OPENHACK_GEODE
            gui::tooltip("DLL injection is not recommended when using Geode.");
#endif
#endif

            if (gui::button("Resources", {0.5f, 0.f}))
                openResources();
            gui::tooltip("Opens the game's resources folder.");
            menu::keybinds::addMenuKeybind("shortcuts.resources", "Resources", []() {
                openResources();
            });

            ImGui::SameLine(0, 2);

            if (gui::button("AppData"))
                openAppData();
            gui::tooltip("Opens the game's save folder.");
            menu::keybinds::addMenuKeybind("shortcuts.appdata", "AppData", []() {
                openAppData();
            });
        });

        // Initialize keybinds
        menu::keybinds::setKeybindCallback("shortcuts.options", []() {
            openOptions();
        });

        menu::keybinds::setKeybindCallback("shortcuts.restart_level", []() {
            restartLevel();
        });

        menu::keybinds::setKeybindCallback("shortcuts.practice_mode", []() {
            togglePractice();
        });

        menu::keybinds::setKeybindCallback("shortcuts.resources", []() {
            openResources();
        });

        menu::keybinds::setKeybindCallback("shortcuts.appdata", []() {
            openAppData();
        });
    }

}