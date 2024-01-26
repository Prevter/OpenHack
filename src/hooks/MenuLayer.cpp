#include "../pch.h"
#include "../hacks/discord_rpc.h"

#include <Geode/modify/MenuLayer.hpp>

namespace hooks
{
    struct MenuLayerHook : geode::Modify<MenuLayerHook, MenuLayer>
    {
        bool init()
        {
            if (!MenuLayer::init())
                return false;

            if (globals::is_december)
            {
                cocos2d::CCParticleSnow *snow = cocos2d::CCParticleSnow::createWithTotalParticles(700);
                this->addChild(snow);
            }

            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);

            return true;
        }
    };
}
