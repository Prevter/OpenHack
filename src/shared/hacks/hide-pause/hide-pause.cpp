#include "hide-pause.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    gd::PauseLayer* g_pauseLayer = nullptr;

    void updatePauseVisibility() {
        if (!g_pauseLayer) return;
        __try {
            bool enabled = config::get<bool>("hack.hide_pause.enabled", false);
            g_pauseLayer->setVisible(!enabled);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            g_pauseLayer = nullptr;
        }
    }

    void HidePause::onInit() {
        // Set the default value
        config::setIfEmpty("hack.hide_pause.enabled", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("hide_pause.enabled", []() {
            bool enabled = !config::get<bool>("hack.hide_pause.enabled");
            config::set("hack.hide_pause.enabled", enabled);
            updatePauseVisibility();
        });
    }

    void HidePause::onDraw() {
        if (gui::checkbox("Hide Pause Menu", "hack.hide_pause.enabled")) {
            updatePauseVisibility();
        }
        gui::tooltip("Makes the pause menu invisible.");
        menu::keybinds::addMenuKeybind("hide_pause.enabled", "Hide Pause Menu", []() {
            bool enabled = !config::get<bool>("hack.hide_pause.enabled", false);
            config::set("hack.hide_pause.enabled", enabled);
            updatePauseVisibility();
        });
    }

    void HidePause::pauseInit(gd::PauseLayer *self) {
        g_pauseLayer = self;
        updatePauseVisibility();
    }

}