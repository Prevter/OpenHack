#include "hooks.hpp"
#include "../../shared/hacks/instant-complete/instant-complete.hpp"
#include "../../shared/hacks/display/display.hpp"
#include "../../shared/hacks/auto-pickup-coins/auto-pickup-coins.hpp"
#include "../../shared/hacks/startpos-switcher/startpos-switcher.hpp"
#include "../../shared/hacks/labels/labels.hpp"

#include <dash/hook/PlayLayer.hpp>

namespace openhack::hooks::PlayLayer {
    bool init(gd::PlayLayer *self, gd::GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        hacks::Display::playLayerInit(level);
        hacks::AutoPickupCoins::initLevel();
        hacks::StartPosSwitcher::initLevel();
        hacks::Labels::playLayerInit();

        if (!hook::PlayLayer::init(self, level, useReplay, dontCreateObjects))
            return false;

        hacks::StartPosSwitcher::lateInitLevel();
        hacks::Labels::playLayerLateInit();
        config::setGlobal("discord_rpc.levelTime", std::time(nullptr));
        return true;
    }

    void resetLevel(gd::PlayLayer *self) {
        hacks::Labels::beforeResetLevel();
        hook::PlayLayer::resetLevel(self);
        hacks::InstantComplete::resetLevel();
        hacks::Display::playLayerReset();
        hacks::AutoPickupCoins::resetLevel();
        hacks::StartPosSwitcher::resetLevel();
        config::setGlobal("fromPercent", self->getCurrentPercentInt());
    }

    void addObject(gd::PlayLayer *self, gd::GameObject *object) {
        hook::PlayLayer::addObject(self, object);
        hacks::AutoPickupCoins::addObject(object);
        hacks::StartPosSwitcher::addObject(object);
    }

    void installHooks() {
        LOG_HOOK(PlayLayer, init);
        LOG_HOOK(PlayLayer, resetLevel);
        LOG_HOOK(PlayLayer, addObject);
    }
}
