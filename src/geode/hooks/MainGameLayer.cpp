#include "../pch.hpp"
#include "../../shared/hacks/menu-gameplay/menu-gameplay.hpp"

#include <Geode/modify/MenuGameLayer.hpp>

namespace openhack::hooks::MenuGameLayerHook {
    struct MenuGameLayerHook : geode::Modify<MenuGameLayerHook, MenuGameLayer> {
        void update(float dt) {
            hacks::MenuGameplay::menuUpdate(m_playerObject);
            MenuGameLayer::update(dt);
        }

        void tryJump(float dt) {
            if (config::get<bool>("hack.menu_gameplay", false))
                return;
            return MenuGameLayer::tryJump(dt);
        }
    };
}

