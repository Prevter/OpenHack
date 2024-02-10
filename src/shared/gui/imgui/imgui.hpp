#pragma once
#include <functional>
#include <stdint.h>

/// @brief Namespace for ImGui hooks, to initialize and draw ImGui.
namespace openhack::imgui
{
    /// @brief Initializes ImGui.
    void initialize();

    /// @brief Deinitializes ImGui.
    void deinitialize();

    /// @brief Called from the swapBuffers/drawScene hook to render ImGui.
    void draw();

    /// @brief Sets the init callback.
    void setInitCallback(std::function<bool()> callback);

    /// @brief Sets the draw callback.
    void setDrawCallback(std::function<void()> callback);

    /// @brief Returns true if ImGui is initialized, false otherwise.
    bool isInitialized();

    /// @brief cocos2d::CCMouseDispatcher::dispatchScrollMSG hook.
    /// @return true if the message should be overridden, false otherwise.
    /// (if overridden - return true, if not - return original function's return value)
    bool CCMouseDispatcher_dispatchScrollMSG(float y, float x);

    /// @brief cocos2d::CCIMEDispatcher::dispatchInsertText hook.
    /// @param original Used to call the original function.
    void CCIMEDispatcher_dispatchInsertText(const char *text, int len, uint32_t keys, std::function<void()> original);

    /// @brief cocos2d::CCIMEDispatcher::dispatchDeleteBackward hook.
    /// @param original Used to call the original function.
    void CCIMEDispatcher_dispatchDeleteBackward(std::function<void()> original);

    /// @brief cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG hook.
    /// @return true if the message should be overridden, false otherwise.
    /// (if overridden - return false, if not - return original function's return value)
    bool CCKeyboardDispatcher_dispatchKeyboardMSG(uint32_t key, bool down, bool v1);

    /// @brief cocos2d::CCTouchDispatcher::touches hook.
    /// @param original Used to call the original function.
    void CCTouchDispatcher_touches(void* touches, void* event, uint32_t type, std::function<void(void *, void *, uint32_t)> original);

    /// @brief cocos2d::CCEGLView::toggleFullscreen hook.
    /// @param original Used to call the original function.
    void CCEGLView_toggleFullScreen(std::function<void()> original);

}