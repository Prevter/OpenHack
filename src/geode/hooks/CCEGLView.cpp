#include "../pch.hpp"
#include <Geode/modify/CCEGLView.hpp>

namespace openhack::hooks
{

#ifdef PLATFORM_WINDOWS

    struct CCEGLViewHook : geode::Modify<CCEGLViewHook, cocos2d::CCEGLView>
    {
        void swapBuffers()
        {
            // openhack::imgui::draw();
            cocos2d::CCEGLView::swapBuffers();
        }
        void toggleFullScreen(bool fullscreen, bool borderless)
        {
            openhack::imgui::CCEGLView_toggleFullScreen(
                [&]()
                {
                    cocos2d::CCEGLView::toggleFullScreen(fullscreen, borderless);
                });
        }
    };

#endif

}