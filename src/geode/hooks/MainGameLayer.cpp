#include "../pch.hpp"
#include "../../shared/hacks/menu-gameplay/menu-gameplay.hpp"

#include <Geode/modify/MenuGameLayer.hpp>

namespace openhack::hooks::MenuGameLayerHook {
    bool tryJump(MenuGameLayer *self, float dt) {
        if (config::get<bool>("hack.menu_gameplay", false))
            return false;
        return reinterpret_cast<gd::MenuGameLayer *>(self)->tryJump(dt);
    }

    struct MenuGameLayerHook2 : geode::Modify<MenuGameLayerHook2, MenuGameLayer> {
        void update(float dt) {
            ON_WINDOWS(hacks::MenuGameplay::menuUpdate(reinterpret_cast<gd::PlayerObject *>(m_playerObject));)
            MenuGameLayer::update(dt);
        }
    };
}

#ifdef PLATFORM_WINDOWS
$execute {
    (void) geode::Mod::get()->hook(
            (void *) gd::findOffset("MenuGameLayer::tryJump"),
            &openhack::hooks::MenuGameLayerHook::tryJump,
            "MenuGameLayer::tryJump",
            tulip::hook::TulipConvention::Thiscall);
}
#endif
