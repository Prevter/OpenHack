#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows you to step through the game frame by frame
    class FrameStepper : public EmbeddedHack {
    public:
        FrameStepper() : EmbeddedHack("Frame Stepper", "frame_stepper") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override;

    public:
        /// @brief GJBaseGameLayer::update hook
        static void gameUpdate(float* dt);
    };

}