#include "openhack.hpp"
#include <imgui.h>

namespace openhack
{
    void initialize()
    {
        gd::init();
        imgui::initialize();
        imgui::setDrawCallback([]()
        {
            ImGui::ShowDemoWindow();
        });
    }

    void deinitialize()
    {
        imgui::deinitialize();
    }
}