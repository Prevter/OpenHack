#include "MenuLayer.h"

namespace hooks::MenuLayer
{
    std::function<void()> on_init;
    bool(__thiscall *init)(cocos2d::CCLayer *self);
    bool __fastcall init_hook(cocos2d::CCLayer *self)
    {
        auto ret = init(self);
        if (on_init)
            on_init();
        return ret;
    };
}
