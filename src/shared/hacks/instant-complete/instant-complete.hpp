#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows to change the game's refresh rate and bypass physics.
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