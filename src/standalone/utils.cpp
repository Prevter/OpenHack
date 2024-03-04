#include "utils.hpp"
#include "pch.hpp"
#include "imgui/imgui_hook.hpp"

namespace openhack::utils {
    std::filesystem::path getGameDirectory() noexcept {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        std::filesystem::path fsPath = path;
        fsPath = fsPath.parent_path();
        return fsPath;
    }

    std::filesystem::path getModSaveDirectory() noexcept {
        return getGameDirectory() / "openhack";
    }

    std::filesystem::path getModAssetsDirectory() noexcept {
        return getGameDirectory() / "openhack";
    }

    std::filesystem::path getModFontsDirectory() noexcept {
        return getModAssetsDirectory() / "fonts";
    }

    std::filesystem::path getModHacksDirectory() noexcept {
        return getModAssetsDirectory() / "hacks";
    }

    void lockTickInput() noexcept {
        ImGuiHook::lockTickInput();
    }

    void resetUI() noexcept {
        ImGuiHook::reload();
    }
}