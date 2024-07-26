#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Removes all decoration from the level
    class SmartStartPos : public EmbeddedHack {
    public:
        SmartStartPos() : EmbeddedHack("Smart StartPos", "smart_startpos") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

        static void updateSmartStartPos();

    public:
        /// @brief PlayLayer::init hook
        static void initLevel();

        /// @brief PlayLayer::addObject hook
        static void addObject(GameObject* object);
    };

}