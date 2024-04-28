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


    INSTALL_NAMESPACE(AppDelegate)
    INSTALL_NAMESPACE(CCDrawNode)
    INSTALL_NAMESPACE(CCEGLView)
    INSTALL_NAMESPACE(CCScheduler)
    INSTALL_NAMESPACE(ChannelControl)
    INSTALL_NAMESPACE(GameStatsManager)
    INSTALL_NAMESPACE(GJBaseGameLayer)
    INSTALL_NAMESPACE(PlayLayer)
    INSTALL_NAMESPACE(PlayerObject)
    INSTALL_NAMESPACE(MenuGameLayer)
    INSTALL_NAMESPACE(EditorUI)
    INSTALL_NAMESPACE(LevelEditorLayer)
    INSTALL_NAMESPACE(HardStreak)

    inline void installHooks() {
        L_TRACE("Installing hooks...");
        MH_Initialize();

        AppDelegate::installHooks();
        CCDrawNode::installHooks();
        CCEGLView::installHooks();
        CCScheduler::installHooks();
        ChannelControl::installHooks();
        GameStatsManager::installHooks();
        GJBaseGameLayer::installHooks();
        PlayLayer::installHooks();
        PlayerObject::installHooks();
        MenuGameLayer::installHooks();
        EditorUI::installHooks();
        LevelEditorLayer::installHooks();
        HardStreak::installHooks();

        MH_EnableHook(MH_ALL_HOOKS);
        L_TRACE("All hooks installed.");
    }
}