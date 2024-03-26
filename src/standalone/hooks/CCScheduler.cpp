#include "hooks.hpp"
#include "../../shared/hacks/speedhack/speedhack.hpp"
#include <dash/hook/cocos/CCScheduler.hpp>

namespace openhack::hooks::CCScheduler {
    void update(cocos2d::CCScheduler* self, float dt) {
        hacks::SpeedHack::update(&dt);
        hook::CCScheduler::update(self, dt);
    }

    void installHooks() {
        LOG_HOOK(CCScheduler, update);
    }
}
