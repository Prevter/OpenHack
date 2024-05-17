#include "hooks.hpp"
#include <dash/hook/GJBaseGameLayer.hpp>
#include <dash/hook/GameObject.hpp>

#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"
#include "../../shared/hacks/frame-stepper/frame-stepper.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"
#include "../../shared/hacks/hitboxes/hitboxes.hpp"
#include "../../shared/hacks/display/display.hpp"

namespace openhack::hooks::GJBaseGameLayer {
    void processCommands(gd::GJBaseGameLayer *self) {
        hacks::Labels::gameUpdate();
        hacks::NoclipLimit::processCommands();
        gd::hook::GJBaseGameLayer::processCommands(self);
        hacks::Hitboxes::processCommands();
        hacks::Zephyrus::GJBaseGameLayerProcessCommands();
    }

    void update(gd::GJBaseGameLayer *self, float dt) {
        hacks::FrameStepper::gameUpdate(&dt);
        hacks::Display::schedulerUpdate(dt, [&](float dt) {
            gd::hook::GJBaseGameLayer::update(self, dt);
        });
    }

    /// This is used to fix slopes killing the player when entering a mirror portal

    static bool s_insideDebugUpdate = false;

    void updateDebugDraw(gd::GJBaseGameLayer *self) {
        s_insideDebugUpdate = gd::PlayLayer::get() != nullptr;
        gd::hook::GJBaseGameLayer::updateDebugDraw(self);
        s_insideDebugUpdate = false;
    }

    void determineSlopeDirection(gd::GameObject *self) {
        if (s_insideDebugUpdate) return;
        gd::hook::GameObject::determineSlopeDirection(self);
    }

    /// ==========================================================================

    void installHooks() {
        LOG_HOOK(GJBaseGameLayer, processCommands);
        LOG_HOOK(GJBaseGameLayer, update);
        LOG_HOOK(GJBaseGameLayer, updateDebugDraw);
        LOG_HOOK(GameObject, determineSlopeDirection);
    }
}
