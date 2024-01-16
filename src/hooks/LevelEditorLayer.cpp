#include "hooks.h"
#include "LevelEditorLayer.h"

#include "../hacks/discord_rpc.h"

namespace hooks::LevelEditorLayer
{
    bool(__thiscall *LevelEditorLayer_init)(robtop::LevelEditorLayer *, robtop::GJGameLevel *, bool);
    bool __fastcall init_hook(robtop::LevelEditorLayer *self, int edx, robtop::GJGameLevel *level, bool v1)
    {
        hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::EDIT, level);
        const bool ret = LevelEditorLayer_init(self, level, v1);
        return ret;
    }

    bool(__thiscall *LevelEditorLayer_onExit)(robtop::LevelEditorLayer *, int);
    bool __fastcall onExit_hook(robtop::LevelEditorLayer *self, int edx, int v1)
    {
        hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);
        const bool ret = LevelEditorLayer_onExit(self, v1);
        return ret;
    }

    void setup()
    {
        hooks::create_hook(
            "LevelEditorLayer::init",
            robtop::LevelEditorLayer_init,
            (void *)init_hook,
            (void **)&LevelEditorLayer_init);

        hooks::create_hook(
            "LevelEditorLayer::onExit",
            robtop::LevelEditorLayer_onExit,
            (void *)onExit_hook,
            (void **)&LevelEditorLayer_onExit);
    }
}
