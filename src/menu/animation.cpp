#include "animation.h"

EaseInAnimation::EaseInAnimation(float duration, float from, float to, float *updateProperty, std::function<void()> callback)
{
    _from = from;
    _to = to;
    _delta = _to - _from;
    _updateProperty = updateProperty;
    _callback = callback;
    _duration = duration;
}

float ease_in(float x)
{
    return 1.0f - cosf(x * M_PI / 2);
}

void EaseInAnimation::update(float dt)
{
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