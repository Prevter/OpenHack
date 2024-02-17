#include "pch.hpp"
#include <imgui-cocos.hpp>

$execute {
    openhack::initialize();
    ImGuiCocos::get()
            .setup(openhack::menu::init)
            .draw([](){
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);
                openhack::menu::draw();
            });
}

$on_mod(Unloaded) {
    ImGuiCocos::get().destroy();
    openhack::deinitialize();
}