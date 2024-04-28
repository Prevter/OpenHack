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

        /// @brief Get a rainbow color for specific parameters
        /// @param speed The speed of the rainbow effect
        /// @param saturation The S (saturation) component of the HSV color
        /// @param value The V (value) component of the HSV color
        /// @param offset The offset in milliseconds for the rainbow effect
        static gui::Color getRainbowColor(float speed, float saturation, float value, float offset);

    public:
        /// @brief PlayLayer::postUpdate / LevelEditorLayer::updateVisibility hook
        static void postUpdate();
    };

}