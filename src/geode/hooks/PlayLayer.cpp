#include "../pch.hpp"
#include "../../shared/hacks/display/display.hpp"

#include <Geode/modify/PlayLayer.hpp>

namespace openhack::hooks {
    struct PlayLayerHook : geode::Modify<PlayLayerHook, PlayLayer> {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            hacks::Display::playLayerInit(reinterpret_cast<gd::GJGameLevel *>(level));

            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;

            return true;
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            hacks::Display::playLayerReset();
        }
    };
}



