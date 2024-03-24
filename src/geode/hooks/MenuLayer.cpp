#include "../pch.hpp"
#include <imgui-cocos.hpp>

#include <Geode/modify/MenuLayer.hpp>

namespace openhack::hooks {
    struct MenuLayerHook : geode::Modify<MenuLayerHook, MenuLayer> {
        bool init() {
            if (!MenuLayer::init())
                return false;

            if (!config::getGlobal<bool>("guiInited", false)) {
                ImGuiCocos::get()
                        .setup(menu::init)
                        .draw([]() {
                            ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);

                            // Add ability for ImGui to capture right click
                            auto &io = ImGui::GetIO();
                            if (utils::isKeyPressed("RMB")) {
                                io.AddMouseButtonEvent(1, true);
                            } else if (utils::isKeyReleased("RMB")) {
                                io.AddMouseButtonEvent(1, false);
                            }

                            io.AddKeyEvent(ImGuiMod_Ctrl, utils::isKeyDown("Ctrl"));
                            io.AddKeyEvent(ImGuiMod_Shift, utils::isKeyDown("Shift"));
                            io.AddKeyEvent(ImGuiMod_Alt, utils::isKeyDown("Alt"));

                            menu::draw();
                        });
                config::setGlobal("guiInited", true);
            }

            return true;
        }
    };
}