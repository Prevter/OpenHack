#include "custom-wave-trail.hpp"
#include "../../menu/menu.hpp"

#include "../rgb-icons/rgb-icons.hpp"

#ifndef OPENHACK_GEODE

#include <cocos2d.h>

#endif

namespace openhack::hacks {

    void CustomWaveTrail::onInit() {
        // Set the default value
        config::setIfEmpty("hack.custom_wave_trail.enabled", false);
        config::setIfEmpty("hack.custom_wave_trail.scale", 1.f);
        config::setIfEmpty("hack.custom_wave_trail.rainbow", false);
        config::setIfEmpty("hack.custom_wave_trail.speed", 0.5f);
        config::setIfEmpty("hack.custom_wave_trail.saturation", 100.f);
        config::setIfEmpty("hack.custom_wave_trail.value", 100.f);
        config::setIfEmpty("hack.custom_wave_trail.custom_color", false);
        config::setIfEmpty("hack.custom_wave_trail.color", gui::Color(255, 255, 255, 255));

        // Initialize keybind
        menu::keybinds::setKeybindCallback("custom_wave_trail.enabled", []() {
            bool enabled = !config::get<bool>("hack.custom_wave_trail.enabled");
            config::set("hack.custom_wave_trail.enabled", enabled);
        });
    }

    void CustomWaveTrail::onDraw() {
        gui::callback([]() {
            gui::tooltip("Change your wave trail size/color");
            menu::keybinds::addMenuKeybind("custom_wave_trail.enabled", "Custom Wave Trail", []() {
                bool enabled = !config::get<bool>("hack.custom_wave_trail.enabled");
                config::set("hack.custom_wave_trail.enabled", enabled);
            });
        });
        gui::toggleSetting("Custom Wave Trail", "hack.custom_wave_trail.enabled", []() {
            gui::width(100);
            gui::inputFloat("Scale", "hack.custom_wave_trail.scale", 0.0f, FLT_MAX, "%.2f");
            gui::tooltip("The scale of the wave trail");
            gui::checkbox("Rainbow", "hack.custom_wave_trail.rainbow");
            gui::tooltip("Enable rainbow wave trail");
            if (config::get<bool>("hack.custom_wave_trail.rainbow")) {
                gui::inputFloat("Speed", "hack.custom_wave_trail.speed", 0.0f, FLT_MAX, "%.2f");
                gui::tooltip("The speed of the rainbow effect");
                gui::inputFloat("Saturation", "hack.custom_wave_trail.saturation", 0.0f, 100.0f, "%.2f");
                gui::tooltip("The saturation of the rainbow effect");
                gui::inputFloat("Value", "hack.custom_wave_trail.value", 0.0f, 100.0f, "%.2f");
                gui::tooltip("The value of the rainbow effect");
            }
            gui::checkbox("Custom Color", "hack.custom_wave_trail.custom_color");
            gui::tooltip("Enable custom color wave trail");
            if (config::get<bool>("hack.custom_wave_trail.custom_color")) {
                gui::colorEdit("Color", "hack.custom_wave_trail.color");
                gui::tooltip("The color of the wave trail");
            }
            gui::width();
        }, ImVec2(0, 0));
    }

    void CustomWaveTrail::updateStroke(gd::HardStreak *streak) {
        if (!config::get<bool>("hack.custom_wave_trail.enabled", false)) return;

        gui::Color target;
        bool changeColor = false;
        if (config::get<bool>("hack.custom_wave_trail.rainbow")) {
            target = RGBIcons::getRainbowColor(
                    config::get<float>("hack.custom_wave_trail.speed") / 10.0f,
                    config::get<float>("hack.custom_wave_trail.saturation") / 100.f,
                    config::get<float>("hack.custom_wave_trail.value") / 100.f,
                    0.0f
            );
            changeColor = true;
        } else if (config::get<bool>("hack.custom_wave_trail.custom_color")) {
            target = config::get<gui::Color>("hack.custom_wave_trail.color");
            changeColor = true;
        }

        if (changeColor) {
            reinterpret_cast<cocos2d::CCDrawNode *>(streak)->setColor({
                static_cast<uint8_t>(target.r * 255),
                static_cast<uint8_t>(target.g * 255),
                static_cast<uint8_t>(target.b * 255)
            });
        }

        auto scale = config::get<float>("hack.custom_wave_trail.scale");
        streak->m_pulseSize() = scale;
    }

}