#pragma once

#include "gui/gui.hpp"
#include "menu/menu.hpp"
#include "menu/keybinds.hpp"
#include "hacks/hacks.hpp"
#include "config.hpp"
#include "utils.hpp"

#include "platform/platform.hpp"
#include <version.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>

#ifdef OPENHACK_GEODE
#include "../geode/pch.hpp"
#include <gd.hpp>
#else

#include "../standalone/pch.hpp"
using namespace gd;

#endif

/// @brief The namespace for the OpenHack mod.
namespace openhack {
    /// @brief Initializes everything.
    /// This function should be called from the entry point of the mod.
    void initialize();

    /// @brief Unloads everything.
    void deinitialize();
}