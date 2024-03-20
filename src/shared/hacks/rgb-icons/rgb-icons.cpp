#include "rgb-icons.hpp"
#include "../../menu/menu.hpp"

#ifndef OPENHACK_GEODE

#include <cocos2d.h>

#endif

namespace openhack::hacks {

    void RGBIcons::onInit() {
        // Set the default value
        config::setIfEmpty("hack.rgb_icons.enabled", false);
        config::setIfEmpty("hack.rgb_icons.speed", 0.5f);
        config::setIfEmpty("hack.rgb_icons.saturation", 100.0f);
        config::setIfEmpty("hack.rgb_icons.value", 100.0f);
        config::setIfEmpty("hack.rgb_icons.secondOffset", 0.0f);
        config::setIfEmpty("hack.rgb_icons.playerOffset", 5000.0f);
        config::setIfEmpty("hack.rgb_icons.applyPrimary", true);
        config::setIfEmpty("hack.rgb_icons.applySecondary", true);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("rgb_icons.enabled", []() {
            bool enabled = !config::get<bool>("hack.rgb_icons.enabled");
            config::set("hack.rgb_icons.enabled", enabled);
        });
    }

    void RGBIcons::onDraw() {
        gui::callback([](){
            gui::tooltip("Makes the player icon have a rainbow effect");
            menu::keybinds::addMenuKeybind("rgb_icons.enabled", "RGB Icons", [](){
                bool enabled = !config::get<bool>("hack.rgb_icons.enabled");
                config::set("hack.rgb_icons.enabled", enabled);
            });
        });
        gui::toggleSetting("RGB Icons", "hack.rgb_icons.enabled", []() {
            gui::width(100);
            gui::inputFloat("Speed", "hack.rgb_icons.speed", 0.0f, FLT_MAX, "%.2f");
            gui::tooltip("The speed of the rainbow effect");
            gui::inputFloat("Saturation", "hack.rgb_icons.saturation", 0.0f, 100.0f, "%.2f");
            gui::tooltip("The saturation of the rainbow effect");
            gui::inputFloat("Value", "hack.rgb_icons.value", 0.0f, 100.0f, "%.2f");
            gui::tooltip("The value of the rainbow effect");
            ImGui::Separator();
            gui::checkbox("Rainbow Color 1", "hack.rgb_icons.applyPrimary");
            gui::checkbox("Rainbow Color 2", "hack.rgb_icons.applySecondary");
            ImGui::Separator();
            gui::inputFloat("Secondary Color Offset", "hack.rgb_icons.secondOffset", 0.0f, FLT_MAX, "%.2f ms");
            gui::tooltip("Offset for the secondary color's rainbow effect (in milliseconds)");
            gui::inputFloat("2nd Player Offset", "hack.rgb_icons.playerOffset", 0.0f, FLT_MAX, "%.2f ms");
            gui::tooltip("Offset for the 2nd player's rainbow effect (in milliseconds)");
            gui::width();
        }, ImVec2(0, 0));
    }

    gui::Color getRainbowColor(float speed, float saturation, float value, float offset) {
        time_t ms = utils::getTime();
        float h = fmodf(ms * speed + offset, 360.0f);
        return gui::Color::fromHSV(h, saturation, value);
    }

    void RGBIcons::update() {
        if (!config::get<bool>("hack.rgb_icons.enabled", false)) return;

        auto *gameLayer = gd::GameManager::sharedState()->m_gameLayer();
        if (!gameLayer) return;

        auto *player1 = gameLayer->m_player1();
        auto *player2 = gameLayer->m_player2();

        auto speed = config::get<float>("hack.rgb_icons.speed") / 10.0f;
        auto saturation = config::get<float>("hack.rgb_icons.saturation") / 100.f;
        auto value = config::get<float>("hack.rgb_icons.value") / 100.f;
        auto secondOffset = config::get<float>("hack.rgb_icons.secondOffset");
        auto applyPrimary = config::get<bool>("hack.rgb_icons.applyPrimary");
        auto applySecondary = config::get<bool>("hack.rgb_icons.applySecondary");

        if (player1) {
            if (applyPrimary) {
                auto color1 = getRainbowColor(speed, saturation, value, 0.0f);
                reinterpret_cast<cocos2d::CCSprite *>(player1)->setColor({static_cast<uint8_t>(color1.r * 255),
                                                                          static_cast<uint8_t>(color1.g * 255),
                                                                          static_cast<uint8_t>(color1.b * 255)});
            }
            if (applySecondary) {
                auto color2 = getRainbowColor(speed, saturation, value, secondOffset);
                reinterpret_cast<cocos2d::CCSprite *>(player1->m_iconSpriteSecondary())->setColor(
                        {static_cast<uint8_t>(color2.r * 255),
                         static_cast<uint8_t>(color2.g * 255),
                         static_cast<uint8_t>(color2.b * 255)});
            }
        }

        if (player2) {
            auto offset = config::get<float>("hack.rgb_icons.playerOffset");
            if (applyPrimary) {
                auto color1 = getRainbowColor(speed, saturation, value, offset);
                reinterpret_cast<cocos2d::CCSprite *>(player2)->setColor({static_cast<uint8_t>(color1.r * 255),
                                                                          static_cast<uint8_t>(color1.g * 255),
                                                                          static_cast<uint8_t>(color1.b * 255)});
            }
            if (applySecondary) {
                auto color2 = getRainbowColor(speed, saturation, value, secondOffset + offset);
                reinterpret_cast<cocos2d::CCSprite *>(player2->m_iconSpriteSecondary())->setColor(
                        {static_cast<uint8_t>(color2.r * 255),
                         static_cast<uint8_t>(color2.g * 255),
                         static_cast<uint8_t>(color2.b * 255)});
            }
        }
    }

}