#pragma once
#include "../pch.h"
#include "../bindings/LevelEditorLayer.h"

namespace hooks::LevelEditorLayer
{
    bool __fastcall init_hook(robtop::LevelEditorLayer *self, int edx, robtop::GJGameLevel *level, bool);
    bool __fastcall onExit_hook(robtop::LevelEditorLayer *self, int edx, int);

    void setup();
}