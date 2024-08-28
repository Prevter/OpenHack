#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Kills the player if they die too much in noclip mode
    class Noclip : public EmbeddedHack {
    public:
        Noclip() : EmbeddedHack("Noclip", "noclip") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override;

    public:
        /// @brief PlayLayer::destroyPlayer hook
        /// @return `true` if the original function should be called
        static bool destroyPlayer(PlayerObject* player, GameObject* object);
    };

}