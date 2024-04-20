#include "hooks.hpp"
#include <dash/hook/EditorUI.hpp>

#include "../../shared/hacks/hide-triggers/hide-triggers.hpp"

namespace openhack::hooks::EditorUI {
    void onPlaytest(gd::EditorUI* self, cocos2d::CCObject* sender) {
        gd::hook::EditorUI::onPlaytest(self, sender);
        hacks::HideTriggers::onPlaytest();
    }

    void installHooks() {
        LOG_HOOK(EditorUI, onPlaytest);
    }
}