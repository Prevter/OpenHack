#pragma once

#include <string>
#include <filesystem>

namespace openhack::utils {
    /// @brief Get the directory where all save files are stored.
    /// It's located in "%LOCALAPPDATA%/GeometryDash/geode/mods/prevter.openhack/"
    std::filesystem::path getModSaveDirectory() noexcept;

    /// @brief Get the directory where all assets are stored.
    /// It's located in "Geometry Dash/geode/unzipped/prevter.openhack/"
    std::filesystem::path getModAssetsDirectory() noexcept;

    /// @brief Get the directory where all fonts are stored.
    /// It's located in "Geometry Dash/geode/unzipped/prevter.openhack/"
    std::filesystem::path getModFontsDirectory() noexcept;

    /// @brief Get the directory where all JSON files for hacks are stored.
    /// It's located in "Geometry Dash/geode/unzipped/prevter.openhack/"
    std::filesystem::path getModHacksDirectory() noexcept;

    /// @brief Block all input from the game for one tick.
    void lockTickInput() noexcept;

    /// @brief Reinitialize ImGui and reset the UI.
    void resetUI();
}