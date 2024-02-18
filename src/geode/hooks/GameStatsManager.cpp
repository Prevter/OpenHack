#include "../pch.hpp"

#include <Geode/modify/GameStatsManager.hpp>

namespace openhack::hooks {
    struct GameStatsManagerHook : geode::Modify<GameStatsManagerHook, GameStatsManager> {
        bool isItemUnlocked(UnlockType a, int b) {
            bool value = GameStatsManager::isItemUnlocked(a, b);
            bool musicUnlocker = config::get("hack.level.music", false);

            if (musicUnlocker && a == UnlockType::GJItem && b == 17)
                return true;

            return value;
        }
    };
}