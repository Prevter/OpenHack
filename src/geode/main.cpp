#include "pch.hpp"
#include <imgui-cocos.hpp>
#include <Geode/loader/SettingEvent.hpp>

$on_mod(Loaded) {
    openhack::initialize();

    // Allow user to change between OpenGL 2.0/3.0
    auto *mod = geode::Mod::get();
    ImGuiCocos::get().setForceLegacy(mod->getSettingValue<bool>("legacy-render"));
    geode::listenForSettingChanges<bool>("legacy-render", [](bool value) {
        ImGuiCocos::get().setForceLegacy(value);
    });
}