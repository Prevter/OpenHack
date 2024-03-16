#include "hooks.hpp"
#include "../imgui/imgui_hook.hpp"
#include <dash/hook/cocos/CCEGLView.hpp>

namespace openhack::hooks::CCEGLView {

    void swapBuffers(cocos2d::CCEGLView *self) {
        ImGuiHook::draw(self);
        hook::CCEGLView::swapBuffers(self);
    }

    void pollEvents(cocos2d::CCEGLView *self) {
        ImGuiHook::handleEvents();
        hook::CCEGLView::pollEvents(self);
    }

    void toggleFullScreen(cocos2d::CCEGLView *self, bool fullscreen, bool borderless) {
        ImGuiHook::destroy();
        hook::CCEGLView::toggleFullScreen(self, fullscreen, borderless);
    }

    void installHooks() {
        LOG_HOOK(CCEGLView, swapBuffers);
        LOG_HOOK(CCEGLView, pollEvents);
        LOG_HOOK(CCEGLView, toggleFullScreen);
    }

}