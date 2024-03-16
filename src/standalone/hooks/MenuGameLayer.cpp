#include "hooks.hpp"
#include <dash/hook/MenuGameLayer.hpp>

#include "../../shared/hacks/menu-gameplay/menu-gameplay.hpp"

namespace openhack::hooks::MenuGameLayer {
    bool tryJump(gd::MenuGameLayer *self, float dt) {
        if (config::get<bool>("hack.menu_gameplay", false))
            return false;
        return gd::hook::MenuGameLayer::tryJump(self, dt);
    }

    void update(gd::MenuGameLayer *self, float dt) {
        hacks::MenuGameplay::menuUpdate(self->m_playerObject());
        gd::hook::MenuGameLayer::update(self, dt);
    }

    void installHooks() {
        LOG_HOOK(MenuGameLayer, tryJump);
        LOG_HOOK(MenuGameLayer, update);
    }
}