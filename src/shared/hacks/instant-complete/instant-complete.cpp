#include "instant-complete.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void InstantComplete::onInit() {
        // Set the default value
        config::setIfEmpty("hack.instant_complete.enabled", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("instant_complete.enabled", []() {
            bool enabled = !config::get<bool>("hack.instant_complete.enabled");
            config::set("hack.instant_complete.enabled", enabled);
        });
    }

    void InstantComplete::onDraw() {
        gui::checkbox("Instant Complete", "hack.instant_complete.enabled");
        gui::tooltip("Completes the level when the level starts.");
        menu::keybinds::addMenuKeybind("instant_complete.enabled", "Instant Complete", []() {
            bool enabled = !config::get<bool>("hack.instant_complete.enabled", false);
            config::set("hack.instant_complete.enabled", enabled);
        });
    }

    bool InstantComplete::isCheating() {
        return config::get<bool>("hack.instant_complete.enabled", false);
    }

    void InstantComplete::resetLevel() {
        if (!config::get<bool>("hack.instant_complete.enabled", false)) return;

        // Get the current PlayLayer
        auto *playLayer = gd::PlayLayer::get();
        if (!playLayer) return;
        auto *level = playLayer->m_level();
        if (level->m_levelLength() == gd::GJLevelLength::Platformer) {
            playLayer->playPlatformerEndAnimationToPos({2, 2}, false);
        } else {
            playLayer->playEndAnimationToPos({2, 2});
        }
    }


}