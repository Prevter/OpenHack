#pragma once
#include "../pch.h"
#include "PlayLayer.h"

namespace robtop
{
    inline const char *GameManager_sharedState_pat = "558BEC6AFF68????64A1000000005051A1????33C5508D45F464A300000000A1????85C075406840050000";
    inline const char *GameManager_getGameVariable_pat = "558BEC83E4F86AFF68????64A1000000005083EC38A1????33C489442430535657A1????33C4508D44244864A3000000008BF98B75088D4C24286A03";
    inline const char *GameManager_setGameVariable_pat = "558BEC83E4F86AFF68????64A1000000005083EC58A1????33C489442450535657A1????33C4508D44246864A3000000008BD9895C2410";

    class GameManager;
    inline robtop::GameManager *(__stdcall *GameManager_sharedState_h)();
    inline bool(__thiscall *GameManager_getGameVariable_h)(robtop::GameManager *manager, const char *key);
    inline void(__thiscall *GameManager_setGameVariable_h)(robtop::GameManager *manager, const char *key, bool value);

    inline const uintptr_t FPS_OFFSET = 900;

    class GameManager
    {
    public:
        inline static GameManager *sharedState()
        {
            if (!GameManager_sharedState_h)
                return nullptr;
            return GameManager_sharedState_h();
        }
        inline float getFps()
        {
            return *(float *)((uintptr_t)this + FPS_OFFSET);
        }
        inline void setFps(float fps)
        {
            *(float *)((uintptr_t)this + FPS_OFFSET) = fps;
        }
        inline bool getGameVariable(const char *key)
        {
            if (!GameManager_getGameVariable_h)
                return false;
            return GameManager_getGameVariable_h(this, key);
        }
        inline void setGameVariable(const char *key, bool value)
        {
            if (!GameManager_setGameVariable_h)
                return;
            GameManager_setGameVariable_h(this, key, value);
        }
    };
}