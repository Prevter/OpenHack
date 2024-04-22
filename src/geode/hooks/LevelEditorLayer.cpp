#include "../pch.hpp"
#include "../../shared/hacks/hide-triggers/hide-triggers.hpp"
#include "../../shared/hacks/rgb-icons/rgb-icons.hpp"

#include <Geode/modify/LevelEditorLayer.hpp>

namespace openhack::hooks {
    struct LevelEditorLayerHook : geode::Modify<LevelEditorLayerHook, LevelEditorLayer> {
        void onStopPlaytest() {
            LevelEditorLayer::onStopPlaytest();
            hacks::HideTriggers::onStopPlaytest();
        }

        void updateVisibility(float dt) {
            LevelEditorLayer::updateVisibility(dt);
            hacks::HideTriggers::updateVisibility();
            hacks::RGBIcons::postUpdate();
        }
    };
}