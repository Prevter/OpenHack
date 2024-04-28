#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Makes the pause menu invisible
    class HidePause : public EmbeddedHack {
    public:
        HidePause() : EmbeddedHack("Hide Pause Menu", "hide_pause") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        /// @brief PauseLayer::updateVisibility hook
        static void pauseInit(gd::PauseLayer *self);

    };

}