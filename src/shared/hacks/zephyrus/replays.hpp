#pragma once

#include "../hacks.hpp"
#include <zephyrus.hpp>

namespace openhack::hacks {

    /// @brief Automatically saves replays
    class Zephyrus : public EmbeddedHack {
    public:
        Zephyrus() : EmbeddedHack("Zephyrus", "zephyrus") {}

        void onInit() override;
        void onDraw() override {}
        void update() override;
        bool isCheating() override;

    public:
        /// @brief PlayerObject::pushButton hook
        static void PlayerObjectPushButton(PlayerObject* player, int buttonIndex);
        /// @brief PlayerObject::releaseButton hook
        static void PlayerObjectReleaseButton(PlayerObject* player, int buttonIndex);
        /// @brief GJBaseGameLayer::processCommands hook
        static void GJBaseGameLayerProcessCommands();
        /// @brief PlayLayer::resetLevel hook
        static void PlayLayerResetLevel();
        /// @brief PlayLayer::playEndAnimationToPos hook
        static void endAnimation();
    };

}