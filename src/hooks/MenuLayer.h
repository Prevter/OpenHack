#pragma once
#include "../pch.h"
#include "../bindings/MenuLayer.h"

namespace hooks::MenuLayer
{
    bool __fastcall init_hook(robtop::MenuLayer *self);

    void setup();
}