#include "auto-kill.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    inline float getCurrentPercent(PlayLayer *playLayer) {
        float percent;
        auto *level = playLayer->m_level;
        auto timestamp = level->m_timestamp;
        if (timestamp > 0) {
            percent = static_cast<float>(playLayer->m_gameState.m_currentProgress) / timestamp * 100.f;
        } else {
            percent = playLayer->m_player1->m_position.x / playLayer->m_levelLength * 100.f;
        }

        if (percent >= 100.f) return 100.f;
        else if (percent <= 0.f) return 0.f;
        else return percent;
    }

    void AutoKill::onInit() {
        // Set the default value
        config::setIfEmpty("hack.auto_kill.enabled", false);
        config::setIfEmpty("hack.auto_kill.use_percentage", true);
        config::setIfEmpty("hack.auto_kill.percentage", 75.0f);
        config::setIfEmpty("hack.auto_kill.use_time", false);
        config::setIfEmpty("hack.auto_kill.time", 90.0f);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("auto_kill.enabled", []() {
            bool enabled = !config::get<bool>("hack.auto_kill.enabled");
            config::set("hack.auto_kill.enabled", enabled);
        });
    }

    void AutoKill::onDraw() {
        gui::callback([]() {
            gui::tooltip("Kills the player at a certain time/percentage.");
            menu::keybinds::addMenuKeybind("auto_kill.enabled", "Auto Kill", []() {
                bool enabled = !config::get<bool>("hack.auto_kill.enabled", false);
                config::set("hack.auto_kill.enabled", enabled);
            });
        });
        gui::toggleSetting("Auto Kill", "hack.auto_kill.enabled", []() {
            gui::width(120);

            gui::checkbox("Use Percentage", "hack.auto_kill.use_percentage");
            gui::inputFloat("Percentage", "hack.auto_kill.percentage", 0.f, 100.f, "%.1f%%");
            gui::tooltip("The percentage at which the player will be killed");

            gui::checkbox("Use time", "hack.auto_kill.use_time");
            gui::inputFloat("Time", "hack.auto_kill.time", 0.f, FLT_MAX, "%.2f sec");
            gui::tooltip("Time since the start of the level at which the player will be killed");

            gui::width();
        }, ImVec2(0, 0), 150);
    }

    void AutoKill::killPlayer() {
        auto *playLayer = PlayLayer::get();
        if (playLayer == nullptr) return;

        // Toggle noclip off to make sure the player dies
        hacks::getHack("level.noclip")->applyPatch(false);
        auto *player = playLayer->m_player1;
        if (player != nullptr && !player->m_isDead) {
            playLayer->destroyPlayer(player, player);
        }
        hacks::getHack("level.noclip")->applyPatch();
    }

    void AutoKill::update() {
        if (!config::get<bool>("hack.auto_kill.enabled", false)) return;

        // Get PlayLayer
        auto *playLayer = PlayLayer::get();
        if (playLayer == nullptr) return;

        float percentage = getCurrentPercent(playLayer);
        double time = playLayer->m_gameState.m_levelTime;

        bool checkPercentage = config::get<bool>("hack.auto_kill.use_percentage");
        bool checkTime = config::get<bool>("hack.auto_kill.use_time");
        bool kill = false;

        if (checkPercentage) {
            auto deathPercentage = config::get<float>("hack.auto_kill.percentage");
            kill |= percentage >= deathPercentage;
        }

        if (checkTime) {
            auto deathTime = config::get<float>("hack.auto_kill.time");
            kill |= time >= deathTime;
        }

        if (kill) killPlayer();
    }

}