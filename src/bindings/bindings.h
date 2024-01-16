#pragma once
#include "GameManager.h"
#include "MenuLayer.h"

namespace robtop
{
    inline void init_binding(const char *name, void **ptr, const char *pat)
    {
        *ptr = (void*)patterns::find_pattern(pat);
        if (!*ptr)
        {
            L_ERROR("Failed to find {}", name);
        }
    }

    inline void init_bindings()
    {
        // GameManager
        init_binding("GameManager::sharedState", (void**)&robtop::GameManager_sharedState_h, robtop::GameManager_sharedState_pat);
        init_binding("GameManager::getGameVariable", (void**)&robtop::GameManager_getGameVariable_h, robtop::GameManager_getGameVariable_pat);
        init_binding("GameManager::setGameVariable", (void**)&robtop::GameManager_setGameVariable_h, robtop::GameManager_setGameVariable_pat);

        // MenuLayer
        init_binding("MenuLayer::init", (void**)&robtop::MenuLayer_init, robtop::MenuLayer_init_pat);

        // PlayLayer
        init_binding("PlayLayer::init", (void**)&robtop::PlayLayer_init, robtop::PlayLayer_init_pat);
        init_binding("PlayLayer::onExit", (void**)&robtop::PlayLayer_onExit, robtop::PlayLayer_onExit_pat);
        init_binding("PlayLayer::resetLevel", (void**)&robtop::PlayLayer_resetLevel, robtop::PlayLayer_resetLevel_pat);
    }

}