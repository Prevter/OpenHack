#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"
#include "../../shared/hacks/frame-stepper/frame-stepper.hpp"
#include "../../shared/hacks/hitboxes/hitboxes.hpp"
#include "../../shared/hacks/display/display.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace openhack::hooks::GJBaseGameLayerHook {
    void processCommands(GJBaseGameLayer *self) {
        hacks::Labels::gameUpdate();
        hacks::NoclipLimit::processCommands();
        reinterpret_cast<gd::GJBaseGameLayer *>(self)->processCommands();
        hacks::Hitboxes::processCommands();
        ON_WINDOWS(hacks::Zephyrus::GJBaseGameLayerProcessCommands();)
    }
}

namespace openhack::hooks {
    struct GJBaseGameLayerHook2 : geode::Modify<GJBaseGameLayerHook2, GJBaseGameLayer> {
        void update(float dt) {
            ON_WINDOWS(hacks::FrameStepper::gameUpdate(&dt);
            hacks::Display::schedulerUpdate(dt, [&](float dt) {
                GJBaseGameLayer::update(dt);
            });)
            ON_ANDROID(GJBaseGameLayer::update(dt);)
        }
    };
}

#ifdef PLATFORM_WINDOWS
$execute {
    (void) geode::Mod::get()->hook(
            (void *) gd::findOffset("GJBaseGameLayer::processCommands"),
            &openhack::hooks::GJBaseGameLayerHook::processCommands,
            "GJBaseGameLayer::processCommands",
            tulip::hook::TulipConvention::Thiscall);
}
#endif
