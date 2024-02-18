#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Hack for changing the game's speed.
    class SpeedHack : public EmbeddedHack {
    public:
        SpeedHack() : EmbeddedHack("SpeedHack", "speedhack") {}

        void onInit() override;
        void onDraw() override {}
        void update() override;
        bool isCheating() override;

    public:
        /// @brief cocos2d::CCScheduler::update hook
        static void update(float *dt);

        /// @brief FMOD::ChannelControl::setVolume hook
        static void setVolume(FMOD::Channel* channel);
    };

}