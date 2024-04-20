#include "accurate-percentage.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    float AccuratePercentage::getPercentage(bool oldEvaluation) {
        auto *playLayer = gd::PlayLayer::get();
        if (!playLayer) return 0.f;

        if (oldEvaluation) {
            float playerX = playLayer->m_player1()->getPositionX();
            float endX = playLayer->m_endPortal()->getPositionX();
            return playerX / endX * 100.f;
        }

        auto *level = playLayer->m_level();
        auto timestamp = level->m_timestamp();

        float percent;
        if (timestamp > 0) {
            percent = static_cast<float>(playLayer->m_gameState().m_stepSpeed()) / timestamp * 100.f;
        } else {
            percent = playLayer->m_player1()->m_position().x / playLayer->m_levelLength() * 100.f;
        }

        if (percent >= 100.f) return 100.f;
        else if (percent <= 0.f) return 0.f;
        return percent;
    }

    void AccuratePercentage::onInit() {
        // Set the default value
        config::setIfEmpty("hack.accurate_percentage.enabled", false);
        config::setIfEmpty("hack.accurate_percentage.normal_mode", true);
        config::setIfEmpty("hack.accurate_percentage.old_eval", false);
        config::setIfEmpty("hack.accurate_percentage.decimal_places", 6.f);
        config::setIfEmpty("hack.accurate_percentage.show_minutes", true);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("accurate_percentage.enabled", []() {
            bool enabled = !config::get<bool>("hack.accurate_percentage.enabled");
            config::set("hack.accurate_percentage.enabled", enabled);
        });
    }

    void AccuratePercentage::onDraw() {
        gui::callback([]() {
            gui::tooltip("Makes percentage/platformer time label more accurate.");
            menu::keybinds::addMenuKeybind("accurate_percentage.enabled", "Auto Deafen", []() {
                bool enabled = !config::get<bool>("hack.accurate_percentage.enabled", false);
                config::set("hack.accurate_percentage.enabled", enabled);
            });
        });
        gui::toggleSetting("Accurate Percentage", "hack.accurate_percentage.enabled", []() {
            gui::checkbox("Normal Mode", "hack.accurate_percentage.normal_mode");
            gui::tooltip("Whether to change the percentage label.");
            gui::checkbox("Old Evaluation", "hack.accurate_percentage.old_eval");
            gui::tooltip("Uses 2.1 method to calculate the percentage.");
            gui::width(40);
            gui::inputFloat("Decimal Places", "hack.accurate_percentage.decimal_places", 0.f, 14.f, "%.0f");
            gui::tooltip("The number of decimal places to show.");
            gui::width();
            gui::checkbox("Show Minutes", "hack.accurate_percentage.show_minutes");
            gui::tooltip("Format time label in platformer mode.");
        }, ImVec2(0, 0), 160);
    }

    void AccuratePercentage::postUpdate() {
        if (!config::get<bool>("hack.accurate_percentage.enabled", false)) return;
        auto* playLayer = gd::PlayLayer::get();
        if (!playLayer) return;

        auto* label = playLayer->m_percentageLabel();
        if (!label) return;

        bool isPlatformer = playLayer->m_level()->isPlatformer();
        if (isPlatformer) {
            double time = playLayer->m_dTime();
            bool showMinutes = config::get<bool>("hack.accurate_percentage.show_minutes");
            if (showMinutes) {
                int minutes = floor(time / 60.0);
                time -= minutes * 60;
                if (minutes > 0) {
                    label->setString(fmt::format("{:01}:{:02.3f}", minutes, time));
                } else {
                    label->setString(fmt::format("{:.3f}", time));
                }
            } else {
                label->setString(fmt::format("{:.3f}", time));
            }
        } else if (config::get<bool>("hack.accurate_percentage.normal_mode")) {
            bool oldEvaluation = config::get<bool>("hack.accurate_percentage.old_eval");
            float percent = getPercentage(oldEvaluation);
            int decimalPlaces = floor(config::get<float>("hack.accurate_percentage.decimal_places", 6.f));
            label->setString(fmt::format("{:.{}f}%", percent, decimalPlaces));
        }
    }

}