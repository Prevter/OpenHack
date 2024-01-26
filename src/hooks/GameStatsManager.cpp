#include "../pch.h"
#include "../hacks/hacks.h"

#include <Geode/modify/GameStatsManager.hpp>

namespace hooks
{
    // hacks::ToggleComponent *musicUnlocker = nullptr;

    // struct GameStatsManagerHook : geode::Modify<GameStatsManagerHook, GameStatsManager>
    // {
    //     bool isItemUnlocked(UnlockType a, int b)
    //     {
    //         bool value = GameStatsManager::isItemUnlocked(a, b);

    //         if (!musicUnlocker)
    //         {
    //             musicUnlocker = hacks::find_component<hacks::ToggleComponent>("level.music");
    //         }

    //         // if (musicUnlocker && musicUnlocker->is_enabled() && a == UnlockType::GJItem && b == 17)
    //         // {
    //         //     return true;
    //         // }

    //         return value;
    //     }
    // };
}
