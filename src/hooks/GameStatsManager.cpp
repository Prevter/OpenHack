#include "hooks.h"
#include "GameStatsManager.h"

#include "../hacks/hacks.h"

namespace hooks::GameStatsManager
{
    hacks::ToggleComponent *musicUnlocker;

    bool(__thiscall *GameStatsManager_isItemUnlocked)(robtop::GameStatsManager *, int, int);
    bool __fastcall isItemUnlocked_hook(robtop::GameStatsManager *self, int, int a, int b)
    {
        const bool ret = GameStatsManager_isItemUnlocked(self, a, b);

        if (!musicUnlocker)
        {
            musicUnlocker = hacks::find_component<hacks::ToggleComponent>("level.music");
        }

        if (musicUnlocker->is_enabled() && a == 12 && b == 17)
        {
            return true;
        }

        return ret;
    }

    void setup()
    {
        hooks::create_hook(
            "GameStatsManager::isItemUnlocked",
            robtop::GameStatsManager_isItemUnlocked,
            (void *)isItemUnlocked_hook,
            (void **)&GameStatsManager_isItemUnlocked);
    }
}
