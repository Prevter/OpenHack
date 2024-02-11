#include "../pch.hpp"

#include "Geode/modify/AppDelegate.hpp"
#include <imgui.h>

namespace openhack::hooks
{
    struct AppDelegateHook : geode::Modify<AppDelegateHook, AppDelegate>
    {
        void applicationWillEnterForeground()
        {
            AppDelegate::applicationWillEnterForeground();
            ImGui::GetIO().ClearInputKeys();
        }
    };
}