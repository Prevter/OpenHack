#include "noclip.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void Noclip::onInit() {
        // Set the default value
        config::setIfEmpty("level.noclip", false);
        config::setIfEmpty("hack.noclip.p1", true);
        config::setIfEmpty("hack.noclip.p2", true);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("level.noclip", []() {
            bool enabled = !config::get<bool>("level.noclip");
            config::set("level.noclip", enabled);
        });
    }

    void Noclip::onDraw() {
        gui::callback([](){
            gui::tooltip("Automatically kills the player when you reach the death limit or accuracy limit");
            menu::keybinds::addMenuKeybind("level.noclip", "Noclip", [](){
                bool enabled = !config::get<bool>("level.noclip");
                config::set("level.noclip", enabled);
            });
        });
        gui::toggleSetting("Noclip", "level.noclip", []() {
            gui::width(150);
            gui::checkbox("Player 1", "hack.noclip.p1");
            gui::tooltip("Whether to enable noclip for player 1");
            gui::checkbox("Player 2", "hack.noclip.p2");
            gui::tooltip("Whether to enable noclip for player 2");
            gui::width();
        }, ImVec2(0, 0), 150);
    }

    bool Noclip::isCheating() {
        return config::get<bool>("level.noclip") && (config::get<bool>("hack.noclip.p1") || config::get<bool>("hack.noclip.p2"));
    }

    bool Noclip::destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer* pl = PlayLayer::get();
        if (!pl) return false;

        if (object == pl->m_anticheatSpike)
            return false;

        if (!config::get<bool>("level.noclip"))
            return false;

        if (config::get<bool>("hack.noclip.p1") && player == pl->m_player1)
            return true;

        if (config::get<bool>("hack.noclip.p2") && player == pl->m_player2)
            return true;

        return false;
    }

}