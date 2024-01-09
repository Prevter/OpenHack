#include "animation.h"

EaseInAnimation::EaseInAnimation(float duration, float from, float to, float *updateProperty, std::function<void()> callback)
{
    _from = from;
    _to = to;
    _delta = _to - _from;
    _updateProperty = updateProperty;
    _callback = callback;
    _duration = duration;
    _time = 0;

    // If the duration is 0, just set the value and call the callback
    if (_duration == 0)
    {
        *_updateProperty = _to;

        if (_callback)
            _callback();
    }
}

float ease_in(float x)
{
    return 1.0f - cosf(x * (float)M_PI / 2.f);
}

void EaseInAnimation::update(float dt)
{
    if (is_done())
        return;

    _time += dt;

    if (_time >= _duration)
    {
        *_updateProperty = _to;

        if (_callback)
            _callback();
    }
    else
    {
        *_updateProperty = _to - _delta * (1 - ease_in(_time / _duration));
    }
}