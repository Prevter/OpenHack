#include "../pch.hpp"

#include <Geode/modify/CCScheduler.hpp>
#include "../../shared/hacks/speedhack/speedhack.hpp"

namespace openhack::hooks {
    struct CCSchedulerHook : geode::Modify<CCSchedulerHook, cocos2d::CCScheduler> {
        void update(float dt) {
            hacks::SpeedHack::update(&dt);
            return cocos2d::CCScheduler::update(dt);
        }
    };
}