#pragma once
#include "../hook.h"

#include "MenuLayer.h"
// #include "PlayLayer.h"

namespace hooks
{
    inline void init_all()
    {
        hook::try_bind_method(
            "MenuLayer::init",
            MenuLayer::init_hook,
            (void **)&MenuLayer::init,
            MenuLayer::init_pattern);
    }
}