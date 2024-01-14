#pragma once
#include "GameManager.h"
#include "MenuLayer.h"

namespace robtop
{

    inline void init_bindings()
    {
        // GameManager
        robtop::getGameVariable_h = (bool(__thiscall *)(robtop::GameManager *, const char *))
            patterns::find_pattern(robtop::getGameVariable_pat);
        robtop::sharedGameManager_h = (robtop::GameManager * (__stdcall *)())
            patterns::find_pattern(robtop::getGameManager_pat);
        robtop::setGameVariable_h = (void(__thiscall *)(robtop::GameManager *, const char *, bool))
            patterns::find_pattern(robtop::setGameVariable_pat);

        if (!robtop::sharedGameManager_h)
        {
            L_ERROR("Failed to find GameManager::sharedState");
        }
        if (!robtop::getGameVariable_h)
        {
            L_ERROR("Failed to find GameManager::getGameVariable");
        }
        if (!robtop::setGameVariable_h)
        {
            L_ERROR("Failed to find GameManager::setGameVariable");
        }

        // MenuLayer
        robtop::MenuLayer_init = (bool(__thiscall *)(robtop::MenuLayer *))
            patterns::find_pattern(robtop::MenuLayer_init_pat);

        if (!robtop::MenuLayer_init)
        {
            L_ERROR("Failed to find MenuLayer::init");
        }
    }

}