#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows to change the game's refresh rate and bypass physics.
    class Display : public EmbeddedHack {
    public:
        Display() : EmbeddedHack("Display", "display") {}

        void onInit() override;
        void onLateInit() override;
        void onDraw() override {}
        void update() override {}
        bool isCheating() override;

        /// @brief Applies all changes to the refresh rate
        static void refreshRate();

        /// @brief Applies physics bypass
        static void refreshPhysics();

    public:
        /// @brief PlayLayer::init hook
        static void playLayerInit(gd::GJGameLevel *level);

        /// @brief PlayLayer::resetLevel hook
        static void playLayerReset();

        /// @brief CCScheduler::update hook
        static void schedulerUpdate(float dt, const std::function<void(float)>& original);
    };

}