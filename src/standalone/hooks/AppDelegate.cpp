#include "hooks.hpp"
#include "../imgui/imgui_hook.hpp"
#include <dash/hook/AppDelegate.hpp>

namespace openhack::hooks::AppDelegate {
    void applicationWillEnterForeground(gd::AppDelegate *self) {
        hook::AppDelegate::applicationWillEnterForeground(self);
        ImGuiHook::clearInput();
    }

    void installHooks() {
        LOG_HOOK(AppDelegate, applicationWillEnterForeground);
    }
}
