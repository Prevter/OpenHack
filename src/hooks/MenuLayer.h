#pragma once
#include "../pch.h"

namespace hooks::MenuLayer
{
    const std::string init_pattern = "558BEC83E4F883EC7453";
    extern std::function<void()> on_init;
    extern bool(__thiscall *init)(cocos2d::CCLayer *self);
    bool __fastcall init_hook(cocos2d::CCLayer *self);
}