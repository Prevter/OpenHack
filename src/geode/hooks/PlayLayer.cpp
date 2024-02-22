#include "../pch.hpp"
#include "../../shared/hacks/instant-complete/instant-complete.hpp"
#include "../../shared/hacks/display/display.hpp"
#include "../../shared/hacks/auto-pickup-coins/auto-pickup-coins.hpp"

#include <Geode/modify/PlayLayer.hpp>

namespace openhack::hooks {
    struct PlayLayerHook : geode::Modify<PlayLayerHook, PlayLayer> {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            hacks::Display::playLayerInit(reinterpret_cast<gd::GJGameLevel *>(level));
            hacks::AutoPickupCoins::initLevel();

            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;

            return true;
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            hacks::InstantComplete::resetLevel();
            hacks::Display::playLayerReset();
            hacks::AutoPickupCoins::resetLevel();
        }

        void addObject(GameObject *object) {
            PlayLayer::addObject(object);
            hacks::AutoPickupCoins::addObject(reinterpret_cast<gd::GameObject *>(object));
        }
    };
}



