#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"
#include "../../shared/hacks/frame-stepper/frame-stepper.hpp"
#include "../../shared/hacks/hitboxes/hitboxes.hpp"
#include "../../shared/hacks/display/display.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameObject.hpp>

namespace openhack::hooks::GJBaseGameLayerHook {
    void processCommands(GJBaseGameLayer *self) {
        hacks::Labels::gameUpdate();
        hacks::NoclipLimit::processCommands();
        reinterpret_cast<gd::GJBaseGameLayer *>(self)->processCommands();
        hacks::Hitboxes::processCommands();
        hacks::Zephyrus::GJBaseGameLayerProcessCommands();
    }
}

namespace openhack::hooks {
    static bool s_insideDebugUpdate = false;

    struct GJBaseGameLayerHook2 : geode::Modify<GJBaseGameLayerHook2, GJBaseGameLayer> {
        void update(float dt) {
            hacks::FrameStepper::gameUpdate(&dt);
            hacks::Display::schedulerUpdate(dt, [&](float dt) {
                GJBaseGameLayer::update(dt);
            });
        }

        // This is used to fix slopes killing the player when entering a mirror portal
        void updateDebugDraw() {
            s_insideDebugUpdate = true;
            GJBaseGameLayer::updateDebugDraw();
            s_insideDebugUpdate = false;
        }
    };

    struct GameObjectBugfixHook : geode::Modify<GameObjectBugfixHook, GameObject> {
        void determineSlopeDirection() {
            if (s_insideDebugUpdate) return;
            GameObject::determineSlopeDirection();
        }
    };
}

$execute {
    (void) geode::Mod::get()->hook(
            (void *) gd::findOffset("GJBaseGameLayer::processCommands"),
            &openhack::hooks::GJBaseGameLayerHook::processCommands,
            "GJBaseGameLayer::processCommands",
            tulip::hook::TulipConvention::Thiscall);
}
