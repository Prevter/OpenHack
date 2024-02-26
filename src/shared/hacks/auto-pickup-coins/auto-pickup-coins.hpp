#pragma once

#include "../hacks.hpp"
#include <vector>

namespace openhack::hacks {

    /// @brief Automatically picks up coins when they are spawned
    class AutoPickupCoins : public EmbeddedHack {
    public:
        AutoPickupCoins() : EmbeddedHack("Auto Pickup Coins", "auto_pickup_coins") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override;

    public:
        /// @brief PlayLayer::init hook
        static void initLevel();
        /// @brief PlayLayer::addObject hook
        static void addObject(gd::GameObject *object);
        /// @brief PlayLayer::resetLevel hook
        static void resetLevel();

    private:
        static std::vector<gd::GameObject*> m_coins;
    };

}