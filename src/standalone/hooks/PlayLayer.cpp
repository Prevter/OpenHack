#include "hooks.hpp"
#include "../../shared/hacks/instant-complete/instant-complete.hpp"
#include "../../shared/hacks/display/display.hpp"
#include "../../shared/hacks/auto-pickup-coins/auto-pickup-coins.hpp"
#include <dash/hook/PlayLayer.hpp>

namespace openhack::hooks::PlayLayer {
    bool init(gd::PlayLayer *self, gd::GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        hacks::Display::playLayerInit(level);
        hacks::AutoPickupCoins::initLevel();

        if (!hook::PlayLayer::init(self, level, useReplay, dontCreateObjects))
            return false;

        return true;
    }

    void resetLevel(gd::PlayLayer *self) {
        hook::PlayLayer::resetLevel(self);
        hacks::InstantComplete::resetLevel();
        hacks::Display::playLayerReset();
        hacks::AutoPickupCoins::resetLevel();
    }

    void addObject(gd::PlayLayer *self, gd::GameObject *object) {
        hook::PlayLayer::addObject(self, object);
        hacks::AutoPickupCoins::addObject(object);
    }

    void installHooks() {
        LOG_HOOK(PlayLayer, init);
        LOG_HOOK(PlayLayer, resetLevel);
        LOG_HOOK(PlayLayer, addObject);
    }
}
