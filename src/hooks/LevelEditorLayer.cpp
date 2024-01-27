#include "../pch.h"
#include "../hacks/discord_rpc.h"
#include "../bindings/LevelEditorLayer.h"
#include "hooks.h"
#include "LevelEditorLayer.h"

#include <Geode/modify/LevelEditorLayer.hpp>

namespace hooks
{
    struct LevelEditorLayerHook : geode::Modify<LevelEditorLayerHook, LevelEditorLayer>
    {
        bool init(GJGameLevel *level, bool v1)
        {
            if (!LevelEditorLayer::init(level, v1))
                return false;

            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::EDIT, level);
            return true;
        }

        // unavailable in geode
        // void onExit()
        // {
        //     LevelEditorLayer::onExit();
        //     hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);
        // }
    };

    namespace LevelEditorLayerHooks
    {
        bool(__thiscall *LevelEditorLayer_onExit)(LevelEditorLayer *, int);
        bool __fastcall onExit_hook(LevelEditorLayer *self, int edx, int v1)
        {
            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);
            const bool ret = LevelEditorLayer_onExit(self, v1);
            return ret;
        }

        void setup()
        {
            hooks::create_hook(
                "LevelEditorLayer::onExit",
                robtop::LevelEditorLayer_onExit,
                (void *)onExit_hook,
                (void **)&LevelEditorLayer_onExit);
        }
    }
}
