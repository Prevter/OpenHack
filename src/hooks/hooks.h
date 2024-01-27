#pragma once
#include "../hook.h"
#include <MinHook.h>

#include "PlayLayer.h"

namespace hooks
{
    inline void create_hook(const char* name, void *method, void *hook, void **original)
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
        PlayLayerHooks::setup();
    }
}