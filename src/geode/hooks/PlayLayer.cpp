#include "../pch.hpp"
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
#include "../../shared/hacks/accurate-percentage/accurate-percentage.hpp"

#include <Geode/modify/PlayLayer.hpp>

namespace openhack::hooks {
    struct PlayLayerHook : geode::Modify<PlayLayerHook, PlayLayer> {
        bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
            ON_WINDOWS(hacks::Display::playLayerInit(reinterpret_cast<gd::GJGameLevel *>(level));
            hacks::AutoPickupCoins::initLevel();
            hacks::StartPosSwitcher::initLevel();
            hacks::Labels::playLayerInit();
            hacks::SmartStartPos::initLevel();)

            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;

            ON_WINDOWS(hacks::StartPosSwitcher::lateInitLevel();
            hacks::Labels::playLayerLateInit();
            config::setGlobal("discord_rpc.levelTime", std::time(nullptr));)
            return true;
        }

        void resetLevel() {
            ON_WINDOWS(hacks::NoclipLimit::resetLevel();
            hacks::Labels::beforeResetLevel();)
            PlayLayer::resetLevel();
            ON_WINDOWS(hacks::Hitboxes::resetLevel();
            hacks::InstantComplete::resetLevel();
            hacks::Display::playLayerReset();
            hacks::AutoPickupCoins::resetLevel();
            hacks::StartPosSwitcher::resetLevel();
            hacks::Zephyrus::PlayLayerResetLevel();
            hacks::RandomSeed::resetLevel();)
            config::setGlobal("fromPercent", getCurrentPercentInt());
        }

        void addObject(GameObject *object) {
            PlayLayer::addObject(object);
            ON_WINDOWS(hacks::AutoPickupCoins::addObject(reinterpret_cast<gd::GameObject *>(object));
            hacks::StartPosSwitcher::addObject(reinterpret_cast<gd::GameObject *>(object));
            hacks::SmartStartPos::addObject(reinterpret_cast<gd::GameObject *>(object));)
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) {
            ON_WINDOWS(hacks::NoclipLimit::destroyPlayer(reinterpret_cast<gd::GameObject *>(object));)
            PlayLayer::destroyPlayer(player, object);
            ON_WINDOWS(hacks::NoclipLimit::postDestroyPlayer();
            hacks::Hitboxes::destroyPlayer();)
        }

        void postUpdate(float dt) {
            PlayLayer::postUpdate(dt);
            ON_WINDOWS(hacks::Hitboxes::postUpdate();
            hacks::AccuratePercentage::postUpdate();)
        }

        void fullReset() {
            PlayLayer::fullReset();
            ON_WINDOWS(hacks::Hitboxes::fullReset();)
        }

        void playEndAnimationToPos(cocos2d::CCPoint pos) {
            ON_WINDOWS(hacks::Zephyrus::endAnimation();)
            PlayLayer::playEndAnimationToPos(pos);
        }

        void playPlatformerEndAnimationToPos(cocos2d::CCPoint pos, bool unk) {
            ON_WINDOWS(hacks::Zephyrus::endAnimation();)
            PlayLayer::playPlatformerEndAnimationToPos(pos, unk);
        }

        void onQuit() {
            ON_WINDOWS(hacks::AutoSave::onLevelQuit();)
            PlayLayer::onQuit();
        }
    };
}



