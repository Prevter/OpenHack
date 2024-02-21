#include "auto-safemode.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    ToggleComponent *AutoSafemode::safeModeComponent;

    void AutoSafemode::onInit() {
        // Set the default value
        config::setIfEmpty("hack.autosafemode.enabled", false);

        // Find the original "Safe Mode" hack component
        safeModeComponent = getHack("universal.safemode");

        // Initialize keybind
        menu::keybinds::setKeybindCallback("autosafemode.enabled", []() {
            bool enabled = !config::get<bool>("hack.autosafemode.enabled");
            config::set("hack.autosafemode.enabled", enabled);
            if (!enabled) safeModeComponent->applyPatch();
        });
    }

    void AutoSafemode::onDraw() {
        auto isCheating = config::getGlobal<bool>("isCheating", false);

        auto label = fmt::format("Auto Safe Mode ({})", isCheating ? "ON" : "OFF");
        if (gui::checkbox(label.c_str(), "hack.autosafemode.enabled")) {
            bool enabled = config::get<bool>("hack.autosafemode.enabled");
            if (!enabled) safeModeComponent->applyPatch();
        }
        gui::tooltip("Automatically enables Safe Mode when you have cheats enabled.");
        menu::keybinds::addMenuKeybind("autosafemode.enabled", "Auto Safe Mode", []() {
            bool enabled = !config::get<bool>("hack.autosafemode.enabled");
            config::set("hack.autosafemode.enabled", enabled);
            if (!enabled) safeModeComponent->applyPatch();
        });
    }

    void AutoSafemode::update() {
        // Check if any cheats are enabled
        bool isCheating = false;

        // Check hacks
        for (auto &hack: hacks::getHacks()) {
            if (hack->isCheat() && hack->isEnabled()) {
                isCheating = true;
                break;
            }
        }

        // If nothing is found, check embedded hacks
        if (!isCheating) {
            for (auto &embeddedHack: hacks::getEmbeddedHacks()) {
                if (embeddedHack->isCheating()) {
                    isCheating = true;
                    break;
                }
            }
        }

        // Update the global variable
        config::setGlobal("isCheating", isCheating);

        // Toggle Safe Mode if cheats are enabled
        if (!safeModeComponent || !config::get<bool>("hack.autosafemode.enabled")) return;

        if (isCheating) {
            safeModeComponent->applyPatch(true);
        } else {
            safeModeComponent->applyPatch();
        }
    }

}