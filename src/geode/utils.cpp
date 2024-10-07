#include "utils.hpp"
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

namespace openhack::utils {
    std::filesystem::path getModSaveDirectory() noexcept {
        return geode::Mod::get()->getSaveDir();
    }

    std::filesystem::path getModAssetsDirectory() noexcept {
        return geode::Mod::get()->getResourcesDir();
    }

    std::filesystem::path getModFontsDirectory() noexcept {
        // geode packs all files into a single directory
        return getModAssetsDirectory();
    }

    std::filesystem::path getModHacksDirectory() noexcept {
        return getModAssetsDirectory();
    }

    void lockTickInput() noexcept {
        ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Blocking);
    }

    void resetUI() {
        ImGuiCocos::get().reload();
    }
}