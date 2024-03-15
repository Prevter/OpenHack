#include "hooks.hpp"
#include <dash/hook/PlayerObject.hpp>

#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"

namespace openhack::hooks::PlayerObject {
    void pushButton(gd::PlayerObject *self, gd::PlayerButton btn) {
        hacks::Zephyrus::PlayerObjectPushButton(self, static_cast<int>(btn));
        hacks::Labels::pushButton(self);
        gd::hook::PlayerObject::pushButton(self, btn);
    }

    void releaseButton(gd::PlayerObject *self, gd::PlayerButton btn) {
        hacks::Zephyrus::PlayerObjectReleaseButton(self, static_cast<int>(btn));
        hacks::Labels::releaseButton(self);
        gd::hook::PlayerObject::releaseButton(self, btn);
    }

    void installHooks() {
        LOG_HOOK(PlayerObject, pushButton);
        LOG_HOOK(PlayerObject, releaseButton);
    }
}