#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Mute Discord when reached a certain percentage in the level
    class AutoDeafen : public EmbeddedHack {
    public:
        AutoDeafen() : EmbeddedHack("Auto Deafen", "auto_deafen") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override { return false; }

        static bool isMuted;
    };

}