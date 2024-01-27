#pragma once
#include "../pch.h"

namespace robtop
{
    class LevelEditorLayer;
    inline const char *LevelEditorLayer_onExit_pat = "51568BF1E8????8BC8E8";
    inline int(__thiscall *LevelEditorLayer_onExit)(LevelEditorLayer *, int);

    class LevelEditorLayer : public cocos2d::CCLayer
    {
    };
}