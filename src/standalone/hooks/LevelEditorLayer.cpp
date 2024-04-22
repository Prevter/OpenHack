#include "hooks.hpp"
#include <dash/hook/LevelEditorLayer.hpp>

#include "../../shared/hacks/hide-triggers/hide-triggers.hpp"
#include "../../shared/hacks/rgb-icons/rgb-icons.hpp"

namespace openhack::hooks::LevelEditorLayer {
    void onStopPlaytest(gd::LevelEditorLayer *self) {
        gd::hook::LevelEditorLayer::onStopPlaytest(self);
        hacks::HideTriggers::onStopPlaytest();
    }

    void updateVisibility(gd::LevelEditorLayer *self, float dt) {
        gd::hook::LevelEditorLayer::updateVisibility(self, dt);
        hacks::HideTriggers::updateVisibility();
        hacks::RGBIcons::postUpdate();
    }

    void installHooks() {
        LOG_HOOK(LevelEditorLayer, onStopPlaytest);
        LOG_HOOK(LevelEditorLayer, updateVisibility);
    }
}