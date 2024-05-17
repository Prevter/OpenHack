#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"
#include "../../shared/hacks/frame-stepper/frame-stepper.hpp"
#include "../../shared/hacks/hitboxes/hitboxes.hpp"
#include "../../shared/hacks/display/display.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameObject.hpp>

namespace openhack::hooks {
    static bool s_insideDebugUpdate = false;

    struct GJBaseGameLayerHook : geode::Modify<GJBaseGameLayerHook, GJBaseGameLayer> {
        void update(float dt) {
            hacks::FrameStepper::gameUpdate(&dt);
            hacks::Display::schedulerUpdate(dt, [&](float dt) {
                GJBaseGameLayer::update(dt);
            });
        }

        // This is used to fix slopes killing the player when entering a mirror portal
        void updateDebugDraw() {
            s_insideDebugUpdate = PlayLayer::get() != nullptr;
            GJBaseGameLayer::updateDebugDraw();
            s_insideDebugUpdate = false;
        }

        void processCommands(float dt) {
            hacks::Labels::gameUpdate();
            hacks::NoclipLimit::processCommands();
            GJBaseGameLayer::processCommands(dt);
            hacks::Hitboxes::processCommands();
            hacks::Zephyrus::GJBaseGameLayerProcessCommands();
        }
    };

    struct GameObjectBugfixHook : geode::Modify<GameObjectBugfixHook, GameObject> {
        void determineSlopeDirection() {
            if (s_insideDebugUpdate) return;
            GameObject::determineSlopeDirection();
        }
    };
}