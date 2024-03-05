#include "hooks.hpp"
#include <dash/hook/PlayerObject.hpp>

#include "../../shared/hacks/labels/labels.hpp"

namespace openhack::hooks::PlayerObject {
    void pushButton(gd::PlayerObject *self, gd::PlayerButton btn) {
        hacks::Labels::pushButton(self);
        gd::hook::PlayerObject::pushButton(self, btn);
    }

    void releaseButton(gd::PlayerObject *self, gd::PlayerButton btn) {
        hacks::Labels::releaseButton(self);
        gd::hook::PlayerObject::releaseButton(self, btn);
    }

    void installHooks() {
        LOG_HOOK(PlayerObject, pushButton);
        LOG_HOOK(PlayerObject, releaseButton);
    }
}