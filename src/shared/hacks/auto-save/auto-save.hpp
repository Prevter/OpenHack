#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Saves the game automatically on certain events.
    class AutoSave : public EmbeddedHack {
    public:
        AutoSave() : EmbeddedHack("Auto Save", "auto_save") {}

        void onInit() override;
        void onDraw() override;
        void update() override { }
        bool isCheating() override { return false; }

        /// @brief Saves the game.
        static void triggerSave();

    public:
        /// @brief PlayLayer::onQuit hook.
        static void onLevelQuit();
    };

}