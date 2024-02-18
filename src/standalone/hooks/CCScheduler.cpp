#include "hooks.hpp"
#include "../../shared/hacks/speedhack/speedhack.hpp"

namespace openhack::hooks::CCScheduler {
    void update(gd::cocos2d::CCScheduler* self, float dt) {
        hacks::SpeedHack::update(&dt);
        return gd::cocos2d::CCScheduler::update(self, dt);
    }

    void installHooks() {
        LOG_HOOK(gd::cocos2d::CCScheduler, update);
    }
}
