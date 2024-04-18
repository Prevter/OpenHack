#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Kills the player at a certain time/percentage.
    class AutoKill : public EmbeddedHack {
    public:
        AutoKill() : EmbeddedHack("Auto Kill", "auto_kill") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override { return false; }

        /// @brief Kills the player if exists
        static void killPlayer();
    };

}