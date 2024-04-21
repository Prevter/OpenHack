#include "../pch.hpp"

#include <Geode/modify/CCScheduler.hpp>
#include "../../shared/hacks/speedhack/speedhack.hpp"
#include "../../shared/hacks/display/display.hpp"

namespace openhack::hooks {
    struct CCSchedulerHook : geode::Modify<CCSchedulerHook, cocos2d::CCScheduler> {
        void update(float dt) {
            ON_WINDOWS(hacks::SpeedHack::update(&dt);)
            cocos2d::CCScheduler::update(dt);
        }
    };
}