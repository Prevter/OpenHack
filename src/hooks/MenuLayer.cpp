#include "hooks.h"
#include "MenuLayer.h"

namespace hooks::MenuLayer
{
    bool(__thiscall *MenuLayer_init_o)(robtop::MenuLayer *self);
    bool __fastcall init_hook(robtop::MenuLayer *self)
    {
        auto ret = MenuLayer_init_o(self);

        // add snow in december
        if (globals::is_december)
        {
            cocos2d::CCParticleSnow *snow = cocos2d::CCParticleSnow::createWithTotalParticles(700);
            self->addChild(snow);
        }

        return ret;
    };

    void setup()
    {
        hooks::create_hook(
            "MenuLayer::init",
            robtop::MenuLayer_init,
            (void*)init_hook,
            (void **)&MenuLayer_init_o);
    }
}
