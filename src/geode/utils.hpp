#pragma once

#include <string>

namespace openhack::utils {
    /// @brief Get the directory where all save files are stored.
    /// It's located in "%LOCALAPPDATA%/GeometryDash/geode/mods/prevter.openhack/"
    std::string getModSaveDirectory() noexcept;

    /// @brief Get the directory where all assets are stored.
    /// It's located in "Geometry Dash/geode/unzipped/prevter.openhack/"
    std::string getModAssetsDirectory() noexcept;

    /// @brief Get the directory where all fonts are stored.
    /// It's located in "Geometry Dash/geode/unzipped/prevter.openhack/"
    std::string getModFontsDirectory() noexcept;

    /// @brief Get the directory where all JSON files for hacks are stored.
    /// It's located in "Geometry Dash/geode/unzipped/prevter.openhack/"
    std::string getModHacksDirectory() noexcept;

    /// @brief Block all input from the game for one tick.
    void lockTickInput() noexcept;

    /// @brief Reinitialize ImGui and reset the UI.
    void resetUI();
}