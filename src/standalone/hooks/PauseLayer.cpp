#include "hooks.hpp"
#include <dash/hook/PauseLayer.hpp>

#include "../../shared/hacks/hide-pause/hide-pause.hpp"

namespace openhack::hooks::HardStreak {
    void customSetup(gd::PauseLayer* self) {
        hacks::HidePause::customSetup(self);
        gd::hook::HardStreak::customSetup(self);
    }

    void installHooks() {
        LOG_HOOK(PauseLayer, customSetup);
    }
}