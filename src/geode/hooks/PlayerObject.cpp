#include "../pch.hpp"
#include "../../shared/hacks/labels/labels.hpp"

#include <Geode/modify/PlayerObject.hpp>

namespace openhack::hooks {
    struct PlayerObjectHook : geode::Modify<PlayerObjectHook, PlayerObject> {
        void pushButton(PlayerButton btn) {
            hacks::Labels::pushButton(reinterpret_cast<gd::PlayerObject*>(this));
            PlayerObject::pushButton(btn);
        }

        void releaseButton(PlayerButton btn) {
            hacks::Labels::releaseButton(reinterpret_cast<gd::PlayerObject*>(this));
            PlayerObject::releaseButton(btn);
        }
    };
}