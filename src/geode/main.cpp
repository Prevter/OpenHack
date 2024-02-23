#include "pch.hpp"
#include <imgui-cocos.hpp>

$on_mod(Loaded) {
    openhack::initialize();
}

$on_mod(Unloaded) {
    ImGuiCocos::get().destroy();
    openhack::deinitialize();
}