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
                config::setGlobal("guiInited", true);
            }

            return true;
        }
    };
}