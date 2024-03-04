#pragma once

#include <string>

namespace openhack::utils {
    /// @brief Get the directory where all save files are stored.
    /// It's located in "Geometry Dash/openhack/"
    std::filesystem::path getModSaveDirectory() noexcept;

    /// @brief Get the directory where all assets are stored.
    /// It's located in "Geometry Dash/openhack"
    std::filesystem::path getModAssetsDirectory() noexcept;

    /// @brief Get the directory where all fonts are stored.
    /// It's located in "Geometry Dash/openhack/fonts"
    std::filesystem::path getModFontsDirectory() noexcept;

    /// @brief Get the directory where all JSON files for hacks are stored.
    /// It's located in "Geometry Dash/openhack/hacks"
    std::filesystem::path getModHacksDirectory() noexcept;

    /// @brief Block all input from the game for one tick.
    void lockTickInput() noexcept;

    /// @brief Reinitialize ImGui and reset the UI.
    void resetUI() noexcept;
}