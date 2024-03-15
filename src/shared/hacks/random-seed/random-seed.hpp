#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows to set a custom random seed for the game
    class RandomSeed : public EmbeddedHack {
    public:
        RandomSeed() : EmbeddedHack("Random Seed", "random_seed") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override;

        /// @brief Get the current random seed
        static uint32_t& getCurrentSeed();

    public:
        /// @brief PlayLayer::resetLevel hook
        static void resetLevel();
    };

}