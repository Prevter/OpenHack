#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Kills the player if they die too much in noclip mode
    class NoclipLimit : public EmbeddedHack {
    public:
        NoclipLimit() : EmbeddedHack("Noclip Limit", "noclip_limit") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        /// @brief PlayLayer::destroyPlayer hook
        static void destroyPlayer(GameObject* object);

        /// @brief PlayLayer::destroyPlayer hook (after the original function)
        static void postDestroyPlayer();

        /// @brief PlayLayer::resetLevel hook
        static void resetLevel();

        /// @brief GJBaseGameLayer::processCommands hook
        static void processCommands();
    };

}