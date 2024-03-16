#include "menu-gameplay.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void MenuGameplay::onInit() {
        config::setIfEmpty("hack.menu_gameplay", false);

        menu::keybinds::setKeybindCallback("menu_gameplay", []() {
            bool enabled = !config::get<bool>("hack.menu_gameplay");
            config::set("hack.menu_gameplay", enabled);
        });
    }

    void MenuGameplay::onDraw() {
        gui::checkbox("Main Menu Gameplay", "hack.menu_gameplay");
        gui::tooltip("Allows you to jump in the main menu.");
        menu::keybinds::addMenuKeybind("menu_gameplay", "Main Menu Gameplay", []() {
            bool enabled = !config::get<bool>("hack.menu_gameplay", false);
            config::set("hack.menu_gameplay", enabled);
        });
    }

    void MenuGameplay::menuUpdate(gd::PlayerObject *player) {
        if (!config::get<bool>("hack.menu_gameplay", false)) return;
        if (!player || player->m_isSpider()) return;

        const char *keys[] = {"LMB", "Up", "W"};
        for (const char *key: keys) {
            if (utils::isKeyPressed(key))
                player->pushButton(gd::PlayerButton::Jump);
            else if (utils::isKeyReleased(key))
                player->releaseButton(gd::PlayerButton::Jump);
        }
    }

}