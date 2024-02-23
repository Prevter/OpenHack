#pragma once

#include "../hacks.hpp"
#include <vector>

namespace openhack::hacks {

    /// @brief Allows to change the game's refresh rate and bypass physics.
    class StartPosSwitcher : public EmbeddedHack {
    public:
        StartPosSwitcher() : EmbeddedHack("StartPos Switcher", "startpos_switch") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override { return false; }

    public:
        /// @brief PlayLayer::init hook (before the original one)
        static void initLevel();

        /// @brief PlayLayer::init hook (after the original one)
        static void lateInitLevel();

        /// @brief PlayLayer::addObject hook
        static void addObject(gd::GameObject *object);

        /// @brief PlayLayer::resetLevel hook
        static void resetLevel();

    private:
        static void pickStartPos(gd::PlayLayer *playLayer, int32_t index);
        static void updateLabel();

        static std::vector<gd::StartPosObject *> m_startposes;
        static int32_t m_currentIndex;
    };

}