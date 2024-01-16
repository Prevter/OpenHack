#include "hooks.h"
#include "PlayLayer.h"

#include "../hacks/discord_rpc.h"

namespace hooks::PlayLayer
{
    bool(__thiscall *PlayLayer_init)(robtop::PlayLayer *, robtop::GJGameLevel *, bool, bool);
    bool __fastcall init_hook(robtop::PlayLayer *self, int edx, robtop::GJGameLevel *level, bool v1, bool v2)
    {
        const bool ret = PlayLayer_init(self, level, v1, v2);
        hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::GAME, level);
        return ret;
    }

    int(__thiscall *PlayLayer_onExit)(robtop::PlayLayer *);
    int __fastcall onQuit_hook(robtop::PlayLayer *self)
    {
        const int ret = PlayLayer_onExit(self);
        hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);
        return ret;
    }

    void(__thiscall *PlayLayer_resetLevel)(robtop::PlayLayer *);
    void __fastcall resetLevel_hook(robtop::PlayLayer *self)
    {
        PlayLayer_resetLevel(self);
        hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::GAME);
    }

    void setup()
    {
        hooks::create_hook(
            "PlayLayer::init",
            robtop::PlayLayer_init,
            (void *)init_hook,
            (void **)&PlayLayer_init);

        hooks::create_hook(
            "PlayLayer::onExit",
            robtop::PlayLayer_onExit,
            (void *)onQuit_hook,
            (void **)&PlayLayer_onExit);

        hooks::create_hook(
            "PlayLayer::resetLevel",
            robtop::PlayLayer_resetLevel,
            (void *)resetLevel_hook,
            (void **)&PlayLayer_resetLevel);
    }
}