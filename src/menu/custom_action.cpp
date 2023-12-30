#include "custom_action.h"

CustomAction *CustomAction::create(float duration, float from, float to, float *updateProperty, std::function<void()> callback)
{
    CustomAction *ret = new CustomAction();
    if (ret->initWithDuration(duration, from, to, updateProperty, callback))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CustomAction::initWithDuration(float duration, float from, float to, float *updateProperty, std::function<void()> callback)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        _from = from;
        _to = to;
        _updateProperty = updateProperty;
        _callback = callback;
        return true;
    }

    return false;
}

void CustomAction::startWithTarget(cocos2d::CCNode *target)
{
    CCActionInterval::startWithTarget(target);
    _delta = _to - _from;
}

void CustomAction::update(float time)
{
    if (_updateProperty == nullptr)
    {
        _updateProperty = &globals::animation;
    }

    *_updateProperty = _to - _delta * (1 - time);
    if (time >= 1 && _callback)
        _callback();
}