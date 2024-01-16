#pragma once
#include "../pch.h"
#include "GJGameLevel.h"

namespace robtop
{
    class LevelEditorLayer;
    inline const char *LevelEditorLayer_init_pat = "558BEC83E4F86AFF68????64A1000000005081EC????A1????33C4898424????5657A1????33C4508D8424????64A3????8BF9897C241C8B45";
    inline bool(__thiscall *LevelEditorLayer_init)(LevelEditorLayer *, GJGameLevel *, bool);
    
    inline const char *LevelEditorLayer_onExit_pat = "51568BF1E8????8BC8E8";
    inline int(__thiscall *LevelEditorLayer_onExit)(LevelEditorLayer *, int);

    class LevelEditorLayer : public cocos2d::CCLayer
    {
    };
}