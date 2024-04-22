#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Kills the player if they die too much in noclip mode
    class RGBIcons : public EmbeddedHack {
    public:
        RGBIcons() : EmbeddedHack("RGB Icons", "rgb_icons") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        /// @brief PlayLayer::postUpdate / LevelEditorLayer::updateVisibility hook
        static void postUpdate();
    };

}