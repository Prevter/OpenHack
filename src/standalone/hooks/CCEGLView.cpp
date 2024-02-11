#include "hooks.hpp"
#include "../imgui/imgui_hook.hpp"

namespace openhack::hooks::CCEGLView
{

    void swapBuffers(gd::cocos2d::CCEGLView *self)
    {
        ImGuiHook::draw(self);
        gd::cocos2d::CCEGLView::swapBuffers(self);
    }

    void pollEvents(gd::cocos2d::CCEGLView *self)
    {
        ImGuiHook::handleEvents();
        gd::cocos2d::CCEGLView::pollEvents(self);
    }

    void toggleFullScreen(gd::cocos2d::CCEGLView *self, bool fullscreen, bool borderless)
    {
        ImGuiHook::destroy();
        gd::cocos2d::CCEGLView::toggleFullScreen(self, fullscreen, borderless);
    }

    void installHooks()
    {
        LOG_HOOK(gd::cocos2d::CCEGLView, swapBuffers);
        LOG_HOOK(gd::cocos2d::CCEGLView, pollEvents);
        LOG_HOOK(gd::cocos2d::CCEGLView, toggleFullScreen);
    }

}