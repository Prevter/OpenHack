#pragma once

#include "../pch.hpp"
#include <MinHook.h>

#define INSTALL_NAMESPACE(class) namespace class { void installHooks(); }

#define LOG_HOOK(class, method)                          \
    auto hook_##method = class::method.hook(method);     \
    if (hook_##method == gd::utils::HookResult::SUCCESS) \
        L_TRACE("Hooked " #class "::" #method " at 0x{:x}", class::method.getAddress());          \
    else                                                 \
        L_WARN("Failed to hook " #class "::" #method ": {}", static_cast<int>(hook_##method))

namespace openhack::hooks {
    INSTALL_NAMESPACE(CCEGLView)
    INSTALL_NAMESPACE(AppDelegate)
    INSTALL_NAMESPACE(CCScheduler)
    INSTALL_NAMESPACE(ChannelControl)
    INSTALL_NAMESPACE(GameStatsManager)

    inline void installHooks() {
        MH_Initialize();

        gd::setHookCreation([](void *target, void *detour, void **original) {
            MH_CreateHook(target, detour, original);
            MH_EnableHook(target);
        });
        gd::setHookRemoval([](void *target) {
            MH_DisableHook(target);
            MH_RemoveHook(target);
        });

        CCEGLView::installHooks();
        AppDelegate::installHooks();
        CCScheduler::installHooks();
        // ChannelControl::installHooks();
        GameStatsManager::installHooks();

        L_TRACE("All hooks installed.");
    }
}