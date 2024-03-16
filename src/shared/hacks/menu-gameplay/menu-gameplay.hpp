#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Kills the player if they die too much in noclip mode
    class MenuGameplay : public EmbeddedHack {
    public:
        MenuGameplay() : EmbeddedHack("Main Menu Gameplay", "menu_gameplay") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        /// @brief MenuGameLayer::update hook
        static void menuUpdate(gd::PlayerObject *player);
    };

}