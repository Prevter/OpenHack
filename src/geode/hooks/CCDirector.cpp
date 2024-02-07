#include "../pch.hpp"
#include <Geode/modify/CCDirector.hpp>

namespace openhack::hooks
{

#ifndef PLATFORM_WINDOWS

    struct CCDirectorHook : geode::Modify<CCDirectorHook, cocos2d::CCDirector>
    {
        void drawScene()
        {
            cocos2d::CCDirector::drawScene();
            openhack::imgui::draw();
        }
    };

#endif

}