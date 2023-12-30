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

        // add snow in december
        if (globals::is_december)
        {
            cocos2d::CCParticleSnow *snow = cocos2d::CCParticleSnow::createWithTotalParticles(700);
            self->addChild(snow);
        }

        return ret;
    };
}
