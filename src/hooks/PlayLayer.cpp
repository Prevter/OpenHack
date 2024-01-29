#include "../pch.h"

#include "../hacks/discord_rpc.h"
#include "../hacks/startpos_switch.h"
#include "../hacks/pickup_coins.h"
#include "../hacks/shortcuts.h"

#include <Geode/modify/PlayLayer.hpp>

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

        void destructor()
        {
            PlayLayer::~PlayLayer();
            hacks::StartposSwitcher::playLayer_destructor(this);
            hacks::Shortcuts::playLayer_destructor(this);
        }
    };
}