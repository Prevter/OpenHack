#include "hooks.hpp"

namespace openhack::hooks::CCEGLView
{

#ifdef PLATFORM_WINDOWS

    void swapBuffers(gd::cocos2d::CCEGLView *self)
    {
        openhack::imgui::draw();
        gd::cocos2d::CCEGLView::swapBuffers(self);
    }

    void toggleFullScreen(gd::cocos2d::CCEGLView *self, bool fullscreen, bool borderless)
    {
        openhack::imgui::CCEGLView_toggleFullScreen(
            [&]()
            {
                gd::cocos2d::CCEGLView::toggleFullScreen(self, fullscreen, borderless);
            });
    }

#endif

    void installHooks()
    {
#ifdef PLATFORM_WINDOWS
        LOG_HOOK(gd::cocos2d::CCEGLView, swapBuffers);
        LOG_HOOK(gd::cocos2d::CCEGLView, toggleFullScreen);
#endif
    }

}