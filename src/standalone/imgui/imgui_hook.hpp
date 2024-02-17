#pragma once

#include <functional>
#include "../pch.hpp"

namespace ImGuiHook {
    /// @brief Called every frame to draw the ImGui UI.
    /// @param view The CCEGLView instance.
    void draw(gd::cocos2d::CCEGLView *view);

    /// @brief Called from pollEvents to capture the mouse and keyboard input.
    void handleEvents();

    /// @brief Destroys the ImGui context.
    void destroy();

    /// @brief Clears the input state.
    void clearInput();

    /// @brief Sets the callback for the initialization of the ImGui context.
    /// This is called after context creation and before the fonts are loaded,
    /// so this can be used to load custom fonts.
    void setInitCallback(std::function<void()> callback);

    /// @brief Sets the callback for the destruction of the ImGui context.
    void setDrawCallback(std::function<void()> callback);

    /// @brief Check if the ImGui context is initialized.
    bool isInitialized();

    /// @brief Block all input from the game for one tick.
    void lockTickInput() noexcept;
}