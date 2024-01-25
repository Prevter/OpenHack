#pragma once
#include "GameManager.h"
#include "MenuLayer.h"
#include "PlayLayer.h"
#include "LevelEditorLayer.h"
#include "GameStatsManager.h"

namespace robtop
{
    // used to create a binding to a function pointer
    inline void init_binding(const char *name, void **ptr, const char *pat)
    {
        *ptr = (void *)patterns::find_pattern(pat);
        if (!*ptr)
        {
            L_ERROR("Failed to find {}", name);
            return;
        }

        L_TRACE("Found {} at GeometryDash.exe+0x{:x}", name, ((uintptr_t)*ptr) - utils::base_addr);
    }

    // used to find the offset of a member variable from code
    inline void init_member_offset(const char *name, uintptr_t *ptr, const char *pat, size_t size)
    {
        uintptr_t addr = patterns::find_pattern(pat);
        if (!addr)
        {
            L_ERROR("Failed to find {}", name);
            *ptr = 0;
            return;
        }

        // read "size" bytes
        auto bytes = new uint8_t[size];
        memcpy(bytes, (void *)addr, size);

        // example: mov ecz, [ebx+2E14h]
        // we have just read the 2E14h part, and got: 14 2E 00 00
        uintptr_t result = 0;
        for (size_t i = 0; i < size; i++)
        {
            result |= bytes[i] << (i * 8);
        }

        *ptr = result;

        L_TRACE("Member offset for {} is 0x{:x}", name, *ptr);
        delete[] bytes;
    }

    inline void init_bindings()
    {
        // GameManager
        init_binding("GameManager::sharedState", (void **)&robtop::GameManager_sharedState_h, robtop::GameManager_sharedState_pat);
        init_binding("GameManager::getGameVariable", (void **)&robtop::GameManager_getGameVariable_h, robtop::GameManager_getGameVariable_pat);
        init_binding("GameManager::setGameVariable", (void **)&robtop::GameManager_setGameVariable_h, robtop::GameManager_setGameVariable_pat);

        // MenuLayer
        init_binding("MenuLayer::init", (void **)&robtop::MenuLayer_init, robtop::MenuLayer_init_pat);

        // PlayLayer
        init_binding("PlayLayer::init", (void **)&robtop::PlayLayer_init, robtop::PlayLayer_init_pat);
        init_binding("PlayLayer::onExit", (void **)&robtop::PlayLayer_onExit, robtop::PlayLayer_onExit_pat);
        init_binding("PlayLayer::resetLevel", (void **)&robtop::PlayLayer_resetLevel, robtop::PlayLayer_resetLevel_pat);
        init_binding("PlayLayer::pickupCoin", (void **)&robtop::PlayLayer_pickupCoin, robtop::PlayLayer_pickupCoin_pat);
        init_binding("PlayLayer::addObject", (void **)&robtop::PlayLayer_addObject, robtop::PlayLayer_addObject_pat);
        init_binding("PlayLayer::destroyObject", (void **)&robtop::PlayLayer_destroyObject, robtop::PlayLayer_destroyObject_pat);
        init_binding("PlayLayer::setStartPosObject", (void **)&robtop::PlayLayer_setStartPosObject, robtop::PlayLayer_setStartPosObject_pat);
        init_binding("PlayLayer::startMusic", (void **)&robtop::PlayLayer_startMusic, robtop::PlayLayer_startMusic_pat);
        init_binding("PlayLayer::removeAllCheckpoints", (void **)&robtop::PlayLayer_removeAllCheckpoints, robtop::PlayLayer_removeAllCheckpoints_pat);
        init_binding("PlayLayer::~PlayLayer", (void **)&robtop::PlayLayer_destructor, robtop::PlayLayer_destructor_pat);
        init_binding("PlayLayer::togglePracticeMode", (void **)&robtop::PlayLayer_togglePracticeMode, robtop::PlayLayer_togglePracticeMode_pat);
        init_member_offset("PlayLayer::startPosCheckpoint", &robtop::PlayLayer_startPosCheckpoint_offset, robtop::PlayLayer_startPosCheckpoint_pat, 4);
        init_member_offset("PlayLayer::practiceMode", &robtop::PlayLayer_practiceMode_offset, robtop::PlayLayer_practiceMode_pat, 4);

        // LevelEditorLayer
        init_binding("LevelEditorLayer::init", (void **)&robtop::LevelEditorLayer_init, robtop::LevelEditorLayer_init_pat);
        init_binding("LevelEditorLayer::onExit", (void **)&robtop::LevelEditorLayer_onExit, robtop::LevelEditorLayer_onExit_pat);

        // GameStatsManager
        init_binding("GameStatsManager::isItemUnlocked", (void **)&robtop::GameStatsManager_isItemUnlocked, robtop::GameStatsManager_isItemUnlocked_pat);
    }

}