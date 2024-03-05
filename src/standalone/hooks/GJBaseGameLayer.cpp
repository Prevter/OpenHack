#include "hooks.hpp"
#include <dash/hook/GJBaseGameLayer.hpp>

#include "../../shared/hacks/labels/labels.hpp"

namespace openhack::hooks::GJBaseGameLayer {
    void processCommands(gd::GJBaseGameLayer *self) {
        hacks::Labels::gameUpdate();
        gd::hook::GJBaseGameLayer::processCommands(self);
    }

    void installHooks() {
        LOG_HOOK(GJBaseGameLayer, processCommands);
    }
}
