#pragma once

#include <cstdint>

namespace openhack::menu::blur {
    /// @brief Compile OpenGL shader for blurring.
    void compileShader();

    /// @brief Get the compiled blur shader handle.
    uint32_t getShader();

    /// @brief Defines which zone should be blurred.
    enum class State {
        /// @brief No blur.
        Off,
        /// @brief Blur the background of the menu.
        Windows,
        /// @brief Blur the entire screen behind the menu.
        Screen
    };

    /// @brief Sets the blur state.
    /// @param state The new blur state.
    void setState(State state);

    /// @brief Gets the current blur state.
    /// @return The current blur state.
    State getState();

    /// @brief Gets rendered frame as a texture.
    /// @return The texture handle.
    uint32_t getCurrentFrame();

    /// @brief Renders the blur effect.
    void render();
}