#pragma once
#include "../pch.h"
#include "../bindings/PlayLayer.h"

namespace hooks::PlayLayer
{
    bool __fastcall init_hook(robtop::PlayLayer *self, robtop::GJGameLevel *level, bool, bool);
    int __fastcall onQuit_hook(robtop::PlayLayer *self);
    void __fastcall resetLevel_hook(robtop::PlayLayer *self);

    void setup();
}