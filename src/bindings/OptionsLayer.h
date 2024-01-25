#pragma once
#include "../pch.h"

namespace robtop
{
    class OptionsLayer;
    inline const char *OptionsLayer_create_pat = ""; // TODO: find pattern
    inline OptionsLayer *(__fastcall *OptionsLayer_create)();

    class OptionsLayer : public cocos2d::CCLayer
    {
    public:
        inline static OptionsLayer *create()
        {
            if (!OptionsLayer_create)
                return nullptr;
            return OptionsLayer_create();
        }
    };
}