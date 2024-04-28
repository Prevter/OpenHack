#include "hooks.hpp"
#include <dash/hook/HardStreak.hpp>

#include "../../shared/hacks/custom-wave-trail/custom-wave-trail.hpp"

namespace openhack::hooks::HardStreak {
    void updateStroke(gd::HardStreak* self, float dt) {
        hacks::CustomWaveTrail::updateStroke(self);
        gd::hook::HardStreak::updateStroke(self, dt);
    }

    void installHooks() {
        LOG_HOOK(HardStreak, updateStroke);
    }
}