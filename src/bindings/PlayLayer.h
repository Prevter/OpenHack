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

    inline const char *PlayLayer_setStartPosObject_pat = "558BEC568B7508578BF939B7?2A";
    inline void(__thiscall *PlayLayer_setStartPosObject)(PlayLayer *, GameObject *);

    inline const char *PlayLayer_startMusic_pat = "558BEC83E4F851568BF180BE?2A";
    inline void(__thiscall *PlayLayer_startMusic)(PlayLayer *);

    inline const char *PlayLayer_removeAllCheckpoints_pat = "558BEC83E4F883EC08565768????8BF9E8????8BC8E8????6A01";
    inline void(__thiscall *PlayLayer_removeAllCheckpoints)(PlayLayer *);

    inline const char *PlayLayer_destructor_pat = "558BEC6AFF68????64A10000000050515657A1????33C5508D45F464A3000000008BF9897DF0C707????C78708010000????C7870C010000????C78710010000????C78714010000????C78718010000????C7873C010000????C787????????C787????????[C787????????]C745";
    inline void(__thiscall *PlayLayer_destructor)(PlayLayer *);

    inline const char *PlayLayer_togglePracticeMode_pat = "558BEC6AFF68????64A10000000050535657A1????33C5508D45F464A3000000008BF98A5D08";
    inline void(__thiscall *PlayLayer_togglePracticeMode)(PlayLayer *, bool);

    // member offset patterns
    inline const char *PlayLayer_startPosCheckpoint_pat = "74208B8B^??0000";
    inline uintptr_t PlayLayer_startPosCheckpoint_offset;

    inline const char *PlayLayer_practiceMode_pat = "E9????8A81^??000084C0";
    inline uintptr_t PlayLayer_practiceMode_offset;

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
        inline void setStartPosObject(GameObject *object)
        {
            if (PlayLayer_setStartPosObject)
                PlayLayer_setStartPosObject(this, object);
        }
        inline void startMusic()
        {
            if (PlayLayer_startMusic)
                PlayLayer_startMusic(this);
        }
        inline void removeAllCheckpoints()
        {
            if (PlayLayer_removeAllCheckpoints)
                PlayLayer_removeAllCheckpoints(this);
        }
        inline void set_startPosCheckpoint(uintptr_t checkpoint)
        {
            if (!PlayLayer_startPosCheckpoint_offset)
                return;

            uintptr_t addr = (uintptr_t)this + PlayLayer_startPosCheckpoint_offset;
            *(uintptr_t *)addr = checkpoint;
        }
        inline bool isPracticeMode()
        {
            if (!PlayLayer_practiceMode_offset)
                return false;

            uintptr_t addr = (uintptr_t)this + PlayLayer_practiceMode_offset;
            return *(bool *)addr;
        }
        inline void togglePracticeMode(bool enable)
        {
            if (PlayLayer_togglePracticeMode)
                PlayLayer_togglePracticeMode(this, enable);
        }
    };
}