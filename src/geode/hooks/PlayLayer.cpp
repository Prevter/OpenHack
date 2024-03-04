#include "../pch.hpp"
#include "../../shared/hacks/instant-complete/instant-complete.hpp"
#include "../../shared/hacks/display/display.hpp"
#include "../../shared/hacks/auto-pickup-coins/auto-pickup-coins.hpp"
#include "../../shared/hacks/startpos-switcher/startpos-switcher.hpp"
#include "../../shared/hacks/labels/labels.hpp"

#include <Geode/modify/PlayLayer.hpp>

namespace openhack::hooks {
    struct PlayLayerHook : geode::Modify<PlayLayerHook, PlayLayer> {
        bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
            hacks::Display::playLayerInit(reinterpret_cast<gd::GJGameLevel *>(level));
            hacks::AutoPickupCoins::initLevel();
            hacks::StartPosSwitcher::initLevel();
            hacks::Labels::playLayerInit();

            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;

            hacks::StartPosSwitcher::lateInitLevel();
            hacks::Labels::playLayerLateInit();
            config::setGlobal("discord_rpc.levelTime", std::time(nullptr));
            return true;
        }

        void resetLevel() {
            hacks::Labels::beforeResetLevel();
            PlayLayer::resetLevel();
            hacks::InstantComplete::resetLevel();
            hacks::Display::playLayerReset();
            hacks::AutoPickupCoins::resetLevel();
            hacks::StartPosSwitcher::resetLevel();
            config::setGlobal("fromPercent", getCurrentPercentInt());
        }

        void addObject(GameObject *object) {
            PlayLayer::addObject(object);
            hacks::AutoPickupCoins::addObject(reinterpret_cast<gd::GameObject *>(object));
            hacks::StartPosSwitcher::addObject(reinterpret_cast<gd::GameObject *>(object));
        }
    };
}



