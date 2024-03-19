#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows to change default respawn time
    class RespawnDelay : public EmbeddedHack {
    public:
        RespawnDelay() : EmbeddedHack("Respawn Delay", "respawn_delay") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        static float delay;
    };

}