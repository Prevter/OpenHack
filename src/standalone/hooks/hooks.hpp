#pragma once

#include "../pch.hpp"
#include <MinHook.h>
#include <dash/hook/hook.hpp>

#define INSTALL_NAMESPACE(class) namespace class { void installHooks(); }

#define LOG_HOOK(class, method)                          \
    auto hook_##method = gd::hook::class::method(method, MH_CreateHook);     \
    if (hook_##method.status == gd::hook::Status::OK) \
        L_TRACE("Hooked " #class "::" #method " at 0x{:x}", (uintptr_t)hook_##method.address);          \
    else                                                 \
        L_WARN("Failed to hook " #class "::" #method ": {}", static_cast<int>(hook_##method.status))

namespace openhack::hooks {
    using namespace gd;

    INSTALL_NAMESPACE(CCEGLView)
    INSTALL_NAMESPACE(AppDelegate)
    INSTALL_NAMESPACE(CCScheduler)
    INSTALL_NAMESPACE(ChannelControl)
    INSTALL_NAMESPACE(GameStatsManager)
    INSTALL_NAMESPACE(GJBaseGameLayer)
    INSTALL_NAMESPACE(PlayLayer)
    INSTALL_NAMESPACE(PlayerObject)
    INSTALL_NAMESPACE(MenuGameLayer)

    inline void installHooks() {
        L_TRACE("Installing hooks...");
        MH_Initialize();

        CCEGLView::installHooks();
        AppDelegate::installHooks();
        CCScheduler::installHooks();
        ChannelControl::installHooks();
        GameStatsManager::installHooks();
        GJBaseGameLayer::installHooks();
        PlayLayer::installHooks();
        PlayerObject::installHooks();
        MenuGameLayer::installHooks();

        MH_EnableHook(MH_ALL_HOOKS);
        L_TRACE("All hooks installed.");
    }
}