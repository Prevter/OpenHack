#pragma once
#include "../hook.h"

#include "MenuLayer.h"
#include "PlayLayer.h"
#include "LevelEditorLayer.h"

namespace hooks
{
    inline void create_hook(std::string name, void *method, void *hook, void **original)
    {
        if (method == nullptr)
        {
            L_ERROR("{} was nullptr", name);
        }
        else if (MH_CreateHook(method, hook, original) == MH_STATUS::MH_OK)
        {
            L_TRACE("Hooked {}", name);
            MH_EnableHook(method);
        }
        else
        {
            L_ERROR("Failed to hook {}", name);
        }
    }

    inline void init_all()
    {
        hooks::MenuLayer::setup();
        hooks::PlayLayer::setup();
        hooks::LevelEditorLayer::setup();
    }
}