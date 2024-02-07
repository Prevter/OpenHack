#pragma once
#include "gui/gui.hpp"

#include "platform/platform.hpp"
#include <version.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

#include <gd.hpp>

/// @brief The namespace for the OpenHack mod.
namespace openhack
{
    /// @brief Initializes everything.
    /// This function should be called from the entry point of the mod.
    void initialize();

    /// @brief Unloads everything.
    void deinitialize();
}