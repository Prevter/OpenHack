#pragma once

#include <functional>
#include <string>
#include <map>

#include "../gui/gui.hpp"
#include "../gui/window.hpp"

namespace openhack::menu {
    /// @brief Open/close the menu
    void toggle();

    /// @return True if the menu is opened
    bool isOpen();

    /// @brief Function for menu initialization. Creates all required windows.
    void init();

    /// @brief Function which is called every frame to draw the menu.
    void draw();

    /// @brief Adds specified window to the draw stack
    /// @param window Window to add
    void addWindow(const std::string &title, const std::function<void()> &onDraw);

    /// @brief Generates a new position for each window, so they don't overlap and are stacked nicely.
    /// @return Map of window pointers and their new positions
    std::map<gui::Window *, ImVec2> getStackedPositions();

    /// @brief Gets new window positions (using `getStackedPositions()`) and animates the windows to their new positions.
    void stackWindows();
}