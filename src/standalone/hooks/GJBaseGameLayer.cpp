#include "hooks.hpp"
#include <dash/hook/GJBaseGameLayer.hpp>

#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"
#include "../../shared/hacks/frame-stepper/frame-stepper.hpp"

namespace openhack::hooks::GJBaseGameLayer {
    void processCommands(gd::GJBaseGameLayer *self) {
        hacks::Labels::gameUpdate();
        hacks::NoclipLimit::processCommands();
        gd::hook::GJBaseGameLayer::processCommands(self);
    }

    void update(gd::GJBaseGameLayer *self, float dt) {
        hacks::FrameStepper::gameUpdate(&dt);
        gd::hook::GJBaseGameLayer::update(self, dt);
    }

    void installHooks() {
        LOG_HOOK(GJBaseGameLayer, processCommands);
        LOG_HOOK(GJBaseGameLayer, update);
    }
}
