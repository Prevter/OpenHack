#include "pch.hpp"
#include <imgui-cocos.hpp>

$execute {
    openhack::initialize();
    ImGuiCocos::get()
            .setup(openhack::menu::init)
            .draw([]() {
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);

                // Add ability for ImGui to capture right click
                auto &io = ImGui::GetIO();
                if (openhack::utils::isKeyPressed("RMB")) {
                    io.AddMouseButtonEvent(1, true);
                } else if (openhack::utils::isKeyReleased("RMB")) {
                    io.AddMouseButtonEvent(1, false);
                }

                openhack::menu::draw();
            });
}

$on_mod(Unloaded) {
    ImGuiCocos::get().destroy();
    openhack::deinitialize();
}