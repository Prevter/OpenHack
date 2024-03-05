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
    }

    void RGBIcons::onDraw() {
        gui::toggleSetting("RGB Icons", "hack.rgb_icons.enabled", []() {
            gui::width(100);
            gui::inputFloat("Speed", "hack.rgb_icons.speed", 0.0f, FLT_MAX, "%.2f");
            gui::tooltip("The speed of the rainbow effect");
            gui::inputFloat("Saturation", "hack.rgb_icons.saturation", 0.0f, 100.0f, "%.2f");
            gui::tooltip("The saturation of the rainbow effect");
            gui::inputFloat("Value", "hack.rgb_icons.value", 0.0f, 100.0f, "%.2f");
            gui::tooltip("The value of the rainbow effect");
            ImGui::Separator();
            gui::inputFloat("Secondary Color Offset", "hack.rgb_icons.secondOffset", 0.0f, FLT_MAX, "%.2f ms");
            gui::tooltip("Offset for the secondary color's rainbow effect (in milliseconds)");
            gui::inputFloat("2nd Player Offset", "hack.rgb_icons.playerOffset", 0.0f, FLT_MAX, "%.2f ms");
            gui::tooltip("Offset for the 2nd player's rainbow effect (in milliseconds)");
            gui::width();
        }, ImVec2(0, 0));
    }

    gui::Color getRainbowColor(float speed, float saturation, float value, float offset) {
        // Calculate the time offset based on the given offset in milliseconds
        auto currentTime = (double)utils::getTime() + offset;

        // Calculate the hue component of the color based on time and speed
        float hue = fmod(speed * currentTime, 360.0f) / 60.0f;
        float chroma = value * saturation;
        float x = chroma * (1 - fabs(fmod(hue, 2) - 1));
        float m = value - chroma;

        // Calculate the RGB components based on the hue
        float r, g, b;
        if (hue >= 0 && hue < 1) {
            r = chroma;
            g = x;
            b = 0;
        } else if (hue >= 1 && hue < 2) {
            r = x;
            g = chroma;
            b = 0;
        } else if (hue >= 2 && hue < 3) {
            r = 0;
            g = chroma;
            b = x;
        } else if (hue >= 3 && hue < 4) {
            r = 0;
            g = x;
            b = chroma;
        } else if (hue >= 4 && hue < 5) {
            r = x;
            g = 0;
            b = chroma;
        } else {
            r = chroma;
            g = 0;
            b = x;
        }

        // Adjust the RGB components based on the value and add the m offset
        r += m;
        g += m;
        b += m;

        // Clamp the values to be within the range [0, 1]
        r = fminf(fmaxf(r, 0), 1);
        g = fminf(fmaxf(g, 0), 1);
        b = fminf(fmaxf(b, 0), 1);

        // Return the resulting color
        return {r, g, b};

        return {r, g, b, 1.f};
    }

    void RGBIcons::update() {
        if (!config::get<bool>("hack.rgb_icons.enabled", false)) return;

        auto *gameLayer = gd::GameManager::sharedState()->m_gameLayer();
        if (!gameLayer) return;

        auto *player1 = gameLayer->m_player1();
        auto *player2 = gameLayer->m_player2();

        auto speed = config::get<float>("hack.rgb_icons.speed", 0.15f) / 10.0f;
        auto saturation = config::get<float>("hack.rgb_icons.saturation", 65.0f);
        auto value = config::get<float>("hack.rgb_icons.value", 65.0f);
        auto secondOffset = config::get<float>("hack.rgb_icons.secondOffset", 0.0f);

        if (player1) {
            auto color1 = getRainbowColor(speed, saturation, value, 0.0f);
            reinterpret_cast<cocos2d::CCSprite *>(player1)->setColor({static_cast<uint8_t>(color1.r * 255),
                                                                      static_cast<uint8_t>(color1.g * 255),
                                                                      static_cast<uint8_t>(color1.b * 255)});
            auto color2 = getRainbowColor(speed, saturation, value, secondOffset);
            reinterpret_cast<cocos2d::CCSprite *>(player1->m_iconSpriteSecondary())->setColor(
                    {static_cast<uint8_t>(color2.r * 255),
                     static_cast<uint8_t>(color2.g * 255),
                     static_cast<uint8_t>(color2.b * 255)});
        }

        if (player2) {
            auto offset = config::get<float>("hack.rgb_icons.playerOffset", 0.0f);
            auto color1 = getRainbowColor(speed, saturation, value, offset);
            reinterpret_cast<cocos2d::CCSprite *>(player2)->setColor({static_cast<uint8_t>(color1.r * 255),
                                                                      static_cast<uint8_t>(color1.g * 255),
                                                                      static_cast<uint8_t>(color1.b * 255)});
            auto color2 = getRainbowColor(speed, saturation, value, secondOffset + offset);
            reinterpret_cast<cocos2d::CCSprite *>(player2->m_iconSpriteSecondary())->setColor(
                    {static_cast<uint8_t>(color2.r * 255),
                     static_cast<uint8_t>(color2.g * 255),
                     static_cast<uint8_t>(color2.b * 255)});
        }
    }

}