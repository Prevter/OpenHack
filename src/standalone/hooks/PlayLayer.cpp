#include "hooks.hpp"
#include "../../shared/hacks/instant-complete/instant-complete.hpp"
#include "../../shared/hacks/display/display.hpp"
#include <dash/hook/PlayLayer.hpp>

namespace openhack::hooks::PlayLayer {
    bool init(gd::PlayLayer *self, gd::GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        hacks::Display::playLayerInit(level);

        if (!hook::PlayLayer::init(self, level, useReplay, dontCreateObjects))
            return false;

        return true;
    }

    void resetLevel(gd::PlayLayer *self) {
        hook::PlayLayer::resetLevel(self);
        hacks::InstantComplete::resetLevel();
        hacks::Display::playLayerReset();
    }

    void installHooks() {
        LOG_HOOK(PlayLayer, init);
        LOG_HOOK(PlayLayer, resetLevel);
    }
}
