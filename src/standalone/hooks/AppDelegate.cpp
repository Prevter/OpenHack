#include "hooks.hpp"
#include "../imgui/imgui_hook.hpp"

namespace openhack::hooks::AppDelegate
{
    void applicationWillEnterForeground(gd::AppDelegate *self)
    {
        gd::AppDelegate::applicationWillEnterForeground(self);
        ImGuiHook::clearInput();
    }

    void installHooks()
    {
        LOG_HOOK(gd::AppDelegate, applicationWillEnterForeground);
    }
}
