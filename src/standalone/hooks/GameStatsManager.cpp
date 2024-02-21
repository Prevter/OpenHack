#include "hooks.hpp"
#include <dash/hook/GameStatsManager.hpp>

namespace openhack::hooks::GameStatsManager {
    bool isItemUnlocked(gd::GameStatsManager *self, int a, int b) {
        bool value = hook::GameStatsManager::isItemUnlocked(self, a, b);
        bool musicUnlocker = config::get("hack.level.music", false);

        if (musicUnlocker && a == 12 && b == 17)
            return true;

        return value;
    }

    void installHooks() {
        LOG_HOOK(GameStatsManager, isItemUnlocked);
    }
}
