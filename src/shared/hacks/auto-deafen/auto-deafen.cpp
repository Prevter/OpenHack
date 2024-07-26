#include "auto-deafen.hpp"
#include "../../menu/menu.hpp"

#include "../accurate-percentage/accurate-percentage.hpp"

namespace openhack::hacks {

    bool AutoDeafen::isMuted = false;

    inline void setState(bool state) {
        if (state == AutoDeafen::isMuted) return;
        utils::pressKey(config::get<std::string>("hack.auto_deafen.mute_key", "Pause"));
        AutoDeafen::isMuted = state;
    }

    void AutoDeafen::onInit() {
        // Set the default value
        config::setIfEmpty("hack.auto_deafen.enabled", false);
        config::setIfEmpty("hack.auto_deafen.start_percentage", 75.0f);
        config::setIfEmpty("hack.auto_deafen.end_percentage", 100.0f);
        config::setIfEmpty("hack.auto_deafen.mute_key", "Pause");
        config::setIfEmpty("hack.auto_deafen.practice", false);
        config::setIfEmpty("hack.auto_deafen.test_mode", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("auto_deafen.enabled", []() {
            bool enabled = !config::get<bool>("hack.auto_deafen.enabled");
            config::set("hack.auto_deafen.enabled", enabled);
        });
    }

    void AutoDeafen::onDraw() {
        gui::callback([]() {
            gui::tooltip("Mutes Discord when reached a certain percentage in the level.");
            menu::keybinds::addMenuKeybind("auto_deafen.enabled", "Auto Deafen", []() {
                bool enabled = !config::get<bool>("hack.auto_deafen.enabled", false);
                config::set("hack.auto_deafen.enabled", enabled);
            });
        });
        gui::toggleSetting("Auto Deafen", "hack.auto_deafen.enabled", []() {
            gui::width(120);
            gui::keybind("Mute Key", "hack.auto_deafen.mute_key");
            gui::inputFloat("Mute Percentage", "hack.auto_deafen.start_percentage", 0.1f, 100.0f, "%.1f%%");
            gui::tooltip("The percentage at which Discord will be muted.");
            gui::inputFloat("Unmute Percentage", "hack.auto_deafen.end_percentage", 0.1f, 100.0f, "%.1f%%");
            gui::tooltip("The percentage at which Discord will be unmuted.");
            gui::width();
            gui::checkbox("Mute in Practice Mode", "hack.auto_deafen.practice");
            gui::tooltip("Whether to mute in practice mode.");
            gui::checkbox("Mute in Test Mode", "hack.auto_deafen.test_mode");
            gui::tooltip("Whether to mute in test mode (start position).");
        }, ImVec2(0, 0), 120);
    }

    void AutoDeafen::update() {
        if (!config::get<bool>("hack.auto_deafen.enabled", false)) return;

        // Get PlayLayer
        auto *playLayer = PlayLayer::get();
        if (playLayer == nullptr) return setState(false);

        // Get the current percentage
        float percentage = AccuratePercentage::getPercentage();

        // Get the mute percentage
        auto startPercentage = config::get<float>("hack.auto_deafen.start_percentage", 75.0f);
        auto endPercentage = config::get<float>("hack.auto_deafen.end_percentage", 100.0f);
        bool practice = config::get<bool>("hack.auto_deafen.practice", false);
        bool testMode = config::get<bool>("hack.auto_deafen.test_mode", false);

        // Ignore practice and test mode if disabled
        if (!practice && playLayer->m_isPracticeMode) return setState(false);
        if (!testMode && playLayer->m_isTestMode) return setState(false);

        // Check if paused, level ended or died
        auto* player = playLayer->m_player1;
        if (playLayer->m_isPaused || playLayer->m_hasCompletedLevel || player->m_isDead)
            return setState(false);

        setState(percentage >= startPercentage && percentage <= endPercentage);
    }

}