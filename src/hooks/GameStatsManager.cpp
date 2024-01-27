#include "hooks.h"
#include "../hacks/hacks.h"

#include <Geode/modify/GameStatsManager.hpp>

namespace hooks
{
    struct GameStatsManagerHook : geode::Modify<GameStatsManagerHook, GameStatsManager>
    {
        bool isItemUnlocked(UnlockType a, int b)
        {
            bool value = GameStatsManager::isItemUnlocked(a, b);
            auto musicUnlocker = hacks::find_component<hacks::ToggleComponent>("level.music");

            if (musicUnlocker && musicUnlocker->is_enabled() && a == UnlockType::GJItem && b == 17)
            {
                return true;
            }

            return value;
        }
    };
}
