#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows you to step through the game frame by frame
    class HideTriggers : public EmbeddedHack {
    public:
        HideTriggers() : EmbeddedHack("Auto Hide Triggers", "hide_triggers") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

    public:
        /// @brief EditorUI::onPlaytest hook
        static void onPlaytest();

        /// @brief LevelEditorLayer::onStopPlaytest hook
        static void onStopPlaytest();

        /// @brief LevelEditorLayer::updateVisibility hook
        static void updateVisibility();
    };

}