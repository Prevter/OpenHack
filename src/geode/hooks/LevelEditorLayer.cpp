#include "../pch.hpp"
#include "../../shared/hacks/hide-triggers/hide-triggers.hpp"

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
        }
    };
}