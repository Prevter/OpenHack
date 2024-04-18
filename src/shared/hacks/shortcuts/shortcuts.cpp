#include "shortcuts.hpp"
#include "../../menu/menu.hpp"

#ifndef OPENHACK_GEODE

#include <cocos2d.h>

#endif

namespace openhack::hacks {

#ifdef PLATFORM_WINDOWS

    void Shortcuts::patchGame() {
        gui::Modal::create("4GB Patch", [](gui::Modal *popup) {
            ImGui::TextWrapped("This patch allows the game to use 4GB, instead of only 2GB.");
            ImGui::TextWrapped("It is highly recommended to install this patch, as "
                               "it resolves some \"Out of memory\"/\"Bad Allocation\" crashes.");

            if (gui::button("Apply Patch", {0.5, 0.f})) {
                bool success = win32::four_gb::patch();
                L_INFO("Patched the game to use 4GB of memory: {}", success);
                if (success) {
                    popup->close();
                    gui::Modal::create("4GB Patch", [](gui::Modal *popup) {
                        ImGui::TextWrapped("Patched the game to use 4GB of memory. Please restart the game.");
                        if (gui::button("Restart")) {
                            ON_STANDALONE( std::exit(0); ) // TODO: Implement proper restart for standalone
                            ON_GEODE( geode::utils::game::restart(); )
                        }
                    });
                } else {
                    popup->close();
                    gui::Modal::create("4GB Patch", "Failed to patch the game. Could not write to the file.");
                }
            }

            ImGui::SameLine(0, 2);

            if (gui::button("Cancel")) {
                popup->close();
            }
        });
    }

#else

    void Shortcuts::patchGame() {
        L_WARN("4GB patch is not supported on this platform.");
    }

#endif

    void uncompleteLevelConfirmed() {
        if (gd::PlayLayer *playLayer = gd::PlayLayer::get()) {
            auto *level = playLayer->m_level();
            auto *statsManager = gd::GameStatsManager::sharedState();
            statsManager->uncompleteLevel(level);

            // Clear progress
            level->m_practicePercent() = 0;
            level->m_normalPercent() = 0;
            level->m_newNormalPercent2() = 0;
            level->m_orbCompletion() = 0;
            level->m_54() = 0;
            level->m_k111() = 0;
            level->m_bestPoints() = 0;
            level->m_bestTime() = 0;

            // Remove coins
            auto *coinDict = reinterpret_cast<cocos2d::CCDictionary *>(statsManager->m_verifiedUserCoins());
            if (!coinDict) {
                L_WARN("coinDict is null");
                return;
            }
            auto coins = level->m_coins();
            for (auto i = 0; i < coins; i++) {
                auto *key = level->getCoinKey(i + 1);
                coinDict->removeObjectForKey(key);
            }

            // Save the level
            gd::GameLevelManager::sharedState()->saveLevel(level);
        }
    }

    void Shortcuts::uncompleteLevel() {
        gui::Modal::create("Uncomplete level", [](gui::Modal *popup) {
            ImGui::TextWrapped("This will clear all progress from the current level (except for orbs).");
            ImGui::TextWrapped("Are you sure you want to uncomplete the level?");

            if (gui::button("Yes", {0.5f, 0.f})) {
                uncompleteLevelConfirmed();
                popup->close();
            }

            ImGui::SameLine(0, 2);

            if (gui::button("Cancel")) {
                popup->close();
            }
        });
    }

    void Shortcuts::openOptions() {
        auto *options = gd::OptionsLayer::create();
        auto *scene = gd::cocos2d::CCDirector::sharedDirector()->getRunningScene();
        scene->addChild(options);
        options->setZOrder(1000);
        options->showLayer(false);
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

            if (gui::button("Uncomplete Level"))
                uncompleteLevel();
            gui::tooltip("Clears all progress from the level. (You need to be in the level to use this.)");
            menu::keybinds::addMenuKeybind("shortcuts.uncomplete_level", "Uncomplete Level", []() {
                uncompleteLevel();
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
#else
            gui::tooltip("Injects a DLL into the game process. Useful for debugging and modding.");
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

        menu::keybinds::setKeybindCallback("shortcuts.uncomplete_level", []() {
            uncompleteLevel();
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