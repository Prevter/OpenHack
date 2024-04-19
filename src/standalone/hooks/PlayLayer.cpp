#include "hooks.hpp"
#include "../../shared/hacks/instant-complete/instant-complete.hpp"
#include "../../shared/hacks/display/display.hpp"
#include "../../shared/hacks/auto-pickup-coins/auto-pickup-coins.hpp"
#include "../../shared/hacks/startpos-switcher/startpos-switcher.hpp"
#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/noclip-limit/noclip-limit.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"
#include "../../shared/hacks/random-seed/random-seed.hpp"
#include "../../shared/hacks/hitboxes/hitboxes.hpp"
#include "../../shared/hacks/smart-startpos/smart-startpos.hpp"
#include "../../shared/hacks/auto-save/auto-save.hpp"

#include <dash/hook/PlayLayer.hpp>

namespace openhack::hooks::PlayLayer {
    bool init(gd::PlayLayer *self, gd::GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        hacks::Display::playLayerInit(level);
        hacks::AutoPickupCoins::initLevel();
        hacks::StartPosSwitcher::initLevel();
        hacks::Labels::playLayerInit();
        hacks::SmartStartPos::initLevel();

        if (!hook::PlayLayer::init(self, level, useReplay, dontCreateObjects))
            return false;

        hacks::StartPosSwitcher::lateInitLevel();
        hacks::Labels::playLayerLateInit();
        config::setGlobal("discord_rpc.levelTime", std::time(nullptr));
        return true;
    }

    void resetLevel(gd::PlayLayer *self) {
        hacks::NoclipLimit::resetLevel();
        hacks::Labels::beforeResetLevel();
        hook::PlayLayer::resetLevel(self);
        hacks::Hitboxes::resetLevel();
        hacks::InstantComplete::resetLevel();
        hacks::Display::playLayerReset();
        hacks::AutoPickupCoins::resetLevel();
        hacks::StartPosSwitcher::resetLevel();
        config::setGlobal("fromPercent", self->getCurrentPercentInt());
        hacks::Zephyrus::PlayLayerResetLevel();
        hacks::RandomSeed::resetLevel();
    }

    void addObject(gd::PlayLayer *self, gd::GameObject *object) {
        hook::PlayLayer::addObject(self, object);
        hacks::AutoPickupCoins::addObject(object);
        hacks::StartPosSwitcher::addObject(object);
        hacks::SmartStartPos::addObject(object);
    }

    void destroyPlayer(gd::PlayLayer *self, gd::PlayerObject* player, gd::GameObject *object) {
        hacks::NoclipLimit::destroyPlayer(object);
        hook::PlayLayer::destroyPlayer(self, player, object);
        hacks::NoclipLimit::postDestroyPlayer();
        hacks::Hitboxes::destroyPlayer();
    }

    void postUpdate(gd::PlayLayer *self, float dt) {
        hook::PlayLayer::postUpdate(self, dt);
        hacks::Hitboxes::postUpdate();
    }

    void fullReset(gd::PlayLayer *self) {
        hook::PlayLayer::fullReset(self);
        hacks::Hitboxes::fullReset();
    }

    void playEndAnimationToPos(gd::PlayLayer *self, cocos2d::CCPoint pos) {
        hacks::Zephyrus::endAnimation();
        hook::PlayLayer::playEndAnimationToPos(self, pos);
    }

    void playPlatformerEndAnimationToPos(gd::PlayLayer *self, cocos2d::CCPoint pos, bool unk) {
        hacks::Zephyrus::endAnimation();
        hook::PlayLayer::playPlatformerEndAnimationToPos(self, pos, unk);
    }

    void onQuit(gd::PlayLayer *self) {
        hacks::AutoSave::onLevelQuit();
        hook::PlayLayer::onQuit(self);
    }

    void installHooks() {
        LOG_HOOK(PlayLayer, init);
        LOG_HOOK(PlayLayer, resetLevel);
        LOG_HOOK(PlayLayer, addObject);
        LOG_HOOK(PlayLayer, destroyPlayer);
        LOG_HOOK(PlayLayer, postUpdate);
        LOG_HOOK(PlayLayer, fullReset);
        LOG_HOOK(PlayLayer, playEndAnimationToPos);
        LOG_HOOK(PlayLayer, playPlatformerEndAnimationToPos);
        LOG_HOOK(PlayLayer, onQuit);
    }
}
