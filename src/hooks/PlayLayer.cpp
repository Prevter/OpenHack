#include "../pch.h"
#include "hooks.h"
#include "PlayLayer.h"

#include "../hacks/discord_rpc.h"
#include "../hacks/startpos_switch.h"
#include "../hacks/pickup_coins.h"
#include "../hacks/shortcuts.h"

#include <Geode/modify/PlayLayer.hpp>
#include "../bindings/PlayLayer.h"

namespace hooks
{
    struct PlayLayerHook : geode::Modify<PlayLayerHook, PlayLayer>
    {
        bool init(GJGameLevel *level, bool v1, bool v2)
        {
            hacks::PickupCoins::playLayer_init(this, level);
            hacks::StartposSwitcher::playLayer_init(this, level);
            hacks::Shortcuts::playLayer_init(this, level);

            if (!PlayLayer::init(level, v1, v2))
                return false;

            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::GAME, level);
            hacks::StartposSwitcher::playLayer_lateInit(this);

            return true;
        }

        void onQuit()
        {
            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);
            PlayLayer::onQuit();
        }

        void resetLevel()
        {
            PlayLayer::resetLevel();
            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::GAME);
            hacks::PickupCoins::playLayer_resetLevel(this);
        }

        void addObject(GameObject *object)
        {
            PlayLayer::addObject(object);

            hacks::StartposSwitcher::playLayer_addObject(this, object);
            hacks::PickupCoins::playLayer_addObject(this, object);
        }

        // unavailable in geode
        // void destructor()
        // {
        //     hacks::StartposSwitcher::playLayer_destructor(this);
        //     hacks::Shortcuts::playLayer_destructor(this);
        // }
    };

    namespace PlayLayerHooks
    {

        void(__thiscall *PlayLayer_destructor)(robtop::PlayLayer *);
        void __fastcall destructor_hook(robtop::PlayLayer *self)
        {
            hacks::StartposSwitcher::playLayer_destructor((PlayLayer*)self);
            hacks::Shortcuts::playLayer_destructor((PlayLayer*)self);

            PlayLayer_destructor(self);
        }

        void setup()
        {
            hooks::create_hook(
                "PlayLayer::~PlayLayer",
                robtop::PlayLayer_destructor,
                (void *)destructor_hook,
                (void **)&PlayLayer_destructor);
        }
    }
}