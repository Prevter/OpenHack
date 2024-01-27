#include "../pch.h"

#include "../hacks/discord_rpc.h"

#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

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
    };

    struct GJBaseGameLayerHook : geode::Modify<GJBaseGameLayerHook, GJBaseGameLayer>
    {
        void onExit()
        {
            hacks::DiscordRPC::change_state(hacks::DiscordRPC::State::MENU);
            GJBaseGameLayer::onExit();
        }
    };
}
