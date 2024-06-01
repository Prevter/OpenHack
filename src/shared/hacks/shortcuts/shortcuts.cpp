#include "shortcuts.hpp"
#include "../../menu/menu.hpp"

#ifndef OPENHACK_GEODE

#include <cocos2d.h>

#endif

namespace openhack::hacks {

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
        if (!gd::PlayLayer::get()) {
            gui::Modal::create("Uncomplete level", "You need to be in a level to use this.");
            return;
        }

        gui::Modal::create(
                "Uncomplete level",
                "This will clear all progress from the current level (except for orbs).\n"
                "Are you sure you want to uncomplete the level?",
                "Yes", "Cancel", [](bool confirmed) {
                    if (confirmed) {
                        uncompleteLevelConfirmed();
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