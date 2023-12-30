#pragma once
#include "../pch.h"

namespace hooks::MenuLayer
{
    const uint32_t init_offset = 0x276700;
    extern std::function<void()> on_init;
    extern bool(__thiscall *init)(cocos2d::CCLayer *self);
    bool __fastcall init_hook(cocos2d::CCLayer *self);
}