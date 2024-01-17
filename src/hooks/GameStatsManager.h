#pragma once
#include "../pch.h"
#include "../bindings/GameStatsManager.h"

namespace hooks::GameStatsManager
{
    bool __fastcall isItemUnlocked_hook(robtop::GameStatsManager *self, int, int a, int b);

    void setup();
}