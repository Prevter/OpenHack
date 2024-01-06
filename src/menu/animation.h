#pragma once
#include "../pch.h"

// Custom animator for the menu open/close animation
// without relying on cocos2d animations
class EaseInAnimation
{
public:
    EaseInAnimation(float duration, float from, float to, float *updateProperty, std::function<void()> callback);
    void update(float dt);
    bool is_done() { return _time >= _duration; }

private:
    float _from, _to, _delta, _time;
    float _duration;
    float *_updateProperty;
    std::function<void()> _callback;
};