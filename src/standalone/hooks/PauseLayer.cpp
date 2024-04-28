#include "hooks.hpp"
#include <dash/hook/PauseLayer.hpp>

#include "../../shared/hacks/hide-pause/hide-pause.hpp"

namespace openhack::hooks::PauseLayer {
    void customSetup(gd::PauseLayer* self) {
        hacks::HidePause::customSetup(self);
        gd::hook::PauseLayer::customSetup(self);
    }

    void installHooks() {
        LOG_HOOK(PauseLayer, customSetup);
    }
}