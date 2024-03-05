#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace openhack::hooks::GJBaseGameLayerHook {
    void processCommands(GJBaseGameLayer *self) {
        hacks::Labels::gameUpdate();
        hacks::NoclipLimit::processCommands();
        reinterpret_cast<gd::GJBaseGameLayer *>(self)->processCommands();
    }
}

$execute {
    (void) geode::Mod::get()->hook(
            (void *) gd::findOffset("GJBaseGameLayer::processCommands"),
            &openhack::hooks::GJBaseGameLayerHook::processCommands,
            "GJBaseGameLayer::processCommands",
            tulip::hook::TulipConvention::Thiscall);
}
