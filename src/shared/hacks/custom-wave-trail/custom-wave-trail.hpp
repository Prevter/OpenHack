#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Change your wave trail size/color
    class CustomWaveTrail : public EmbeddedHack {
    public:
        CustomWaveTrail() : EmbeddedHack("Custom Wave Trail", "custom_wave_trail") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        /// @brief PlayLayer::postUpdate hook
        static void updateStroke(HardStreak* streak);
    };

}