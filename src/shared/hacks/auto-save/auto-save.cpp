#include "auto-save.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void AutoSave::onInit() {
        // Set the default value
        config::setIfEmpty("hack.auto_save.enabled", false);
        config::setIfEmpty("hack.auto_save.prompt_on_save", true);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("auto_save.enabled", []() {
            bool enabled = !config::get<bool>("hack.auto_save.enabled");
            config::set("hack.auto_save.enabled", enabled);
        });
    }

    void AutoSave::onDraw() {
        gui::callback([]() {
            gui::tooltip("Automatically saves the game on certain events,\n"
                         "which can save you from losing progress on game crashes.");
            menu::keybinds::addMenuKeybind("auto_save.enabled", "Auto Save", []() {
                bool enabled = !config::get<bool>("hack.auto_save.enabled", false);
                config::set("hack.auto_save.enabled", enabled);
            });
        });
        gui::toggleSetting("Auto Save", "hack.auto_save.enabled", []() {
            gui::checkbox("Prompt on Save", "hack.auto_save.prompt_on_save");
            gui::tooltip("Prompts you to save the game before saving.\n"
                         "(The game may lag for a moment.)");
        }, ImVec2(0, 0), 160);
    }

    void AutoSave::triggerSave() {
        bool prompt = config::get<bool>("hack.auto_save.prompt_on_save");
        if (!prompt) {
            gd::AppDelegate::get()->trySaveGame(true);
            return;
        }

        gui::Modal::create(
                "Auto Save",
                "Do you want to save the game?",
                "Yes", "No", [](bool confirmed) {
                    if (confirmed) {
                        gd::AppDelegate::get()->trySaveGame(true);
                    }
                });
    }

    void AutoSave::onLevelQuit() {
        if (!config::get<bool>("hack.auto_save.enabled")) return;

        auto *playLayer = gd::PlayLayer::get();
        if (!playLayer || playLayer->m_level()->m_levelID().value() == 0)
            return;

        triggerSave();
    }

}