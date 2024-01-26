#include "../pch.h"
#include "../hook.h"

#include <Geode/modify/CCEGLView.hpp>

namespace hooks
{
    struct CCEGLViewHook : geode::Modify<CCEGLViewHook, cocos2d::CCEGLView>
    {
        void swapBuffers()
        {
            hook::on_draw(this);
            cocos2d::CCEGLView::swapBuffers();
        }
    };
}
