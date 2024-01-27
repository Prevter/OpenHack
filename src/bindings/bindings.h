#pragma once
#include "PlayLayer.h"
#include "GameObject.h"
#include "LevelEditorLayer.h"

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

        L_TRACE("Found {} at GeometryDash.exe+0x{:x}", name, ((uintptr_t)*ptr) - geode::base::get());
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
        // PlayLayer
        init_binding("PlayLayer::pickupCoin", (void **)&robtop::PlayLayer_pickupCoin, robtop::PlayLayer_pickupCoin_pat);
        init_binding("PlayLayer::removeAllCheckpoints", (void **)&robtop::PlayLayer_removeAllCheckpoints, robtop::PlayLayer_removeAllCheckpoints_pat);
        init_binding("PlayLayer::~PlayLayer", (void **)&robtop::PlayLayer_destructor, robtop::PlayLayer_destructor_pat);
        init_member_offset("PlayLayer::startPosCheckpoint", &robtop::PlayLayer_startPosCheckpoint_offset, robtop::PlayLayer_startPosCheckpoint_pat, 4);

        // GameObject
        init_member_offset("GameObject::objectID", &robtop::GameObject_objectID_offset, robtop::GameObject_objectID_pat, 4);

        // LevelEditorLayer
        init_binding("LevelEditorLayer::onExit", (void **)&robtop::LevelEditorLayer_onExit, robtop::LevelEditorLayer_onExit_pat);
    }

}