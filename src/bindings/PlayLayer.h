#pragma once
#include "../pch.h"
#include "GameObject.h"

namespace robtop
{
    class PlayLayer;
    inline const char *PlayLayer_removeAllCheckpoints_pat = "558BEC83E4F883EC08565768????8BF9E8????8BC8E8????6A01";
    inline void(__thiscall *PlayLayer_removeAllCheckpoints)(PlayLayer *);

    inline const char *PlayLayer_pickupCoin_pat = "558BEC6AFF68????64A1000000005083EC20A1????33C58945EC535657508D45F464A3000000008BD98B7D08FFB754060000";
    inline void(__thiscall *PlayLayer_pickupCoin)(PlayLayer *, GameObject *);

    // inline const char *PlayLayer_destructor_pat = "558BEC6AFF68????64A10000000050515657A1????33C5508D45F464A3000000008BF9897DF0C707????C78708010000????C7870C010000????C78710010000????C78714010000????C78718010000????C7873C010000????C787????????C787????????[C787????????]C745";
    // inline void(__thiscall *PlayLayer_destructor)(PlayLayer *);

    // member offset patterns
    inline const char *PlayLayer_startPosCheckpoint_pat = "74208B8B^??0000";
    inline uintptr_t PlayLayer_startPosCheckpoint_offset;

    class PlayLayer : public cocos2d::CCLayer
    {
    public:
        inline void set_startPosCheckpoint(uintptr_t checkpoint)
        {
            if (!PlayLayer_startPosCheckpoint_offset)
                return;

            uintptr_t addr = (uintptr_t)this + PlayLayer_startPosCheckpoint_offset;
            *(uintptr_t *)addr = checkpoint;
        }
        inline void removeAllCheckpoints()
        {
            if (PlayLayer_removeAllCheckpoints)
                PlayLayer_removeAllCheckpoints(this);
        }
        inline void pickupCoin(GameObject *coin)
        {
            if (PlayLayer_pickupCoin)
                PlayLayer_pickupCoin(this, coin);
        }
    };
}