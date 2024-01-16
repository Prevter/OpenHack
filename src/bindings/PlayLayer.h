#pragma once
#include "../pch.h"

#include "GJGameLevel.h"

namespace robtop
{
    class PlayLayer;
    inline const char *PlayLayer_init_pat = "558BEC83E4C06AFF68????64A1000000005081EC??0000A1????33C489[8424?]???535657A1????33C4508D8424????64A3000000008BD98B7D08";
    inline bool(__thiscall *PlayLayer_init)(PlayLayer *, GJGameLevel *, bool, bool);

    inline const char *PlayLayer_onExit_pat = "5156[8B35????]578BF9[FFD68BC88B10FF52]?C7";
    inline int(__thiscall *PlayLayer_onExit)(PlayLayer *);

    inline const char *PlayLayer_resetLevel_pat = "558BEC83E4C06AFF68????64A1000000005083EC68A1????33C489442430535657A1????33C4508D44247864A3000000008BD9";
    inline void(__thiscall *PlayLayer_resetLevel)(PlayLayer *);

    class PlayLayer : public cocos2d::CCLayer
    {
        inline void resetLevel()
        {
            if (PlayLayer_resetLevel)
                PlayLayer_resetLevel(this);
        }
    };
}