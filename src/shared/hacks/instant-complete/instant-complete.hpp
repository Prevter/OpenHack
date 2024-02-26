#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Instantly completes the level when you start it
    class InstantComplete : public EmbeddedHack {
    public:
        InstantComplete() : EmbeddedHack("Instant Complete", "instant_complete") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override;

    public:
        /// @brief PlayLayer::resetLevel hook
        static void resetLevel();
    };

}