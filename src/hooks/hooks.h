#pragma once
#include "../hook.h"

#include "MenuLayer.h"
#include "PlayLayer.h"

namespace hooks
{
    inline void create_hook(std::string name, void *method, void *hook, void **original)
    {
        if (method == nullptr)
        {
            L_WARN("Failed to hook " + name);
        }
        else
        {
            MH_CreateHook(method, hook, original);
            MH_EnableHook(method);
        }
    }

    inline void init_all()
    {
        hooks::MenuLayer::setup();
        hooks::PlayLayer::setup();
    }
}