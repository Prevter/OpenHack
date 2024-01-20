#pragma once
#include "../pch.h"

#include "GJGameLevel.h"
#include "GameObject.h"

namespace robtop
{
    class PlayLayer;
    inline const char *PlayLayer_init_pat = "558BEC83E4C06AFF68????64A1000000005081EC??0000A1????33C489[8424?]???535657A1????33C4508D8424????64A3000000008BD98B7D08";
    inline bool(__thiscall *PlayLayer_init)(PlayLayer *, GJGameLevel *, bool, bool);

    inline const char *PlayLayer_onExit_pat = "5156[8B35????]578BF9[FFD68BC88B10FF52]?C7";
    inline int(__thiscall *PlayLayer_onExit)(PlayLayer *);

    inline const char *PlayLayer_resetLevel_pat = "558BEC83E4C06AFF68????64A1000000005083EC68A1????33C489442430535657A1????33C4508D44247864A3000000008BD9";
    inline void(__thiscall *PlayLayer_resetLevel)(PlayLayer *);

    inline const char *PlayLayer_pickupCoin_pat = "558BEC6AFF68????64A1000000005083EC20A1????33C58945EC535657508D45F464A3000000008BD98B7D08FFB754060000";
    inline void(__thiscall *PlayLayer_pickupCoin)(PlayLayer *, GameObject *);

    inline const char *PlayLayer_addObject_pat = "558BEC83EC20538B5D0856578BF9897DF085DB0F84";
    inline void(__thiscall *PlayLayer_addObject)(PlayLayer *, GameObject *);

    inline const char *PlayLayer_destroyObject_pat = "558BEC53568B7508578BF98BCE8B06FF90";
    inline void(__thiscall *PlayLayer_destroyObject)(PlayLayer *, GameObject *);

    class PlayLayer : public cocos2d::CCLayer
    {
    public:
        inline void resetLevel()
        {
            if (PlayLayer_resetLevel)
                PlayLayer_resetLevel(this);
        }
        inline void pickupCoin(GameObject *coin)
        {
            if (PlayLayer_pickupCoin)
                PlayLayer_pickupCoin(this, coin);
        }
        inline void destroyObject(GameObject *object)
        {
            if (PlayLayer_destroyObject)
                PlayLayer_destroyObject(this, object);
        }
        inline void addObject(GameObject *object)
        {
            if (PlayLayer_addObject)
                PlayLayer_addObject(this, object);
        }
    };
}