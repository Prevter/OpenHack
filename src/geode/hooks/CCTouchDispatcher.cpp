#include "../pch.hpp"
#include <Geode/modify/CCTouchDispatcher.hpp>

namespace openhack::hooks
{

    struct CCTouchDispatcherHook : geode::Modify<CCTouchDispatcherHook, cocos2d::CCTouchDispatcher>
    {
        void touches(cocos2d::CCSet *touches, cocos2d::CCEvent *event, unsigned int type)
        {
            if (!openhack::imgui::isInitialized() || !touches)
                return cocos2d::CCTouchDispatcher::touches(touches, event, type);

            openhack::imgui::CCTouchDispatcher_touches(
                touches, event, type,
                [&](void *touches_, void *event_, uint32_t type_)
                {
                    cocos2d::CCTouchDispatcher::touches(
                        reinterpret_cast<cocos2d::CCSet *>(touches_),
                        reinterpret_cast<cocos2d::CCEvent *>(event_),
                        type_);
                });
        }
    };

}