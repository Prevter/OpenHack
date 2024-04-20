#include "../pch.hpp"
#include "../../shared/hacks/hide-triggers/hide-triggers.hpp"

#include <Geode/modify/EditorUI.hpp>

namespace openhack::hooks {
    struct EditorUIHook : geode::Modify<EditorUIHook, EditorUI> {
        void onPlaytest(cocos2d::CCObject* sender) {
            EditorUI::onPlaytest(sender);
            hacks::HideTriggers::onPlaytest();
        }
    };
}