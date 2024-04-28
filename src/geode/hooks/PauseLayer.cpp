#include "../pch.hpp"

#include "../../shared/hacks/hide-pause/hide-pause.hpp"

#include <Geode/modify/PauseLayer.hpp>

namespace openhack::hooks {
    struct PauseLayerHook : geode::Modify<PauseLayerHook, PauseLayer> {
        void customSetup() {
            hacks::HidePause::pauseInit(reinterpret_cast<gd::PauseLayer*>(this));
            PauseLayer::customSetup();
        }
    };
}