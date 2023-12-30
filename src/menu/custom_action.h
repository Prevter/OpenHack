#pragma once
#include "../pch.h"
#include <functional>

class CustomAction : public cocos2d::CCActionInterval
{
public:
    virtual void update(float time) override;
    static CustomAction *create(float duration, float from, float to, float *updateProperty, std::function<void()> callback);
    bool initWithDuration(float duration, float from, float to, float *updateProperty, std::function<void()> callback);
    void startWithTarget(cocos2d::CCNode *target) override;

private:
    float _from, _to, _delta;
    float *_updateProperty;
    std::function<void()> _callback;
};