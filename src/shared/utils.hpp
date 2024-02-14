#pragma once

#include <random>
#include <imgui.h>

#ifdef __clang__
#include <ctime>
#else

#include <time.h>

#endif

namespace openhack::utils {
    /// @brief Generates a random number between min and max.
    /// @param min The minimum value.
    /// @param max The maximum value.
    /// @return Random number between min and max.
    inline int random(int min, int max) {
        return min + (rand() % (max - min + 1));
    }

    /// @brief Initializes the random number generator.
    inline void initRandom() {
        srand(time(nullptr));
    }

    /// @brief Get the time since the last frame.
    /// @return Time since the last frame in seconds.
    inline double getDeltaTime() {
        return ImGui::GetIO().DeltaTime;
    }
}