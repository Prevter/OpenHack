#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"
#include "../../shared/hacks/zephyrus/replays.hpp"

#include <Geode/modify/PlayerObject.hpp>

namespace openhack::hooks {
    struct PlayerObjectHook : geode::Modify<PlayerObjectHook, PlayerObject> {
        void pushButton(PlayerButton btn) {
            hacks::Zephyrus::PlayerObjectPushButton(reinterpret_cast<gd::PlayerObject*>(this), static_cast<int>(btn));
            hacks::Labels::pushButton(reinterpret_cast<gd::PlayerObject*>(this));
            PlayerObject::pushButton(btn);
        }

        void releaseButton(PlayerButton btn) {
            hacks::Zephyrus::PlayerObjectReleaseButton(reinterpret_cast<gd::PlayerObject*>(this), static_cast<int>(btn));
            hacks::Labels::releaseButton(reinterpret_cast<gd::PlayerObject*>(this));
            PlayerObject::releaseButton(btn);
        }
    };
}