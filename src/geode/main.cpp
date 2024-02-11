#include "pch.hpp"
#include <imgui-cocos.hpp>

$execute
{
    openhack::initialize();
    ImGuiCocos::get()
        .setup(openhack::gui::init)
        .draw(openhack::gui::draw);
}

$on_mod(Unloaded)
{
    ImGuiCocos::get().destroy();
    openhack::deinitialize();
}