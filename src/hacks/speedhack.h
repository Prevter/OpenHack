#pragma once
#include "../pch.h"
#include "hacks.h"

namespace hacks
{
    extern float speed;
    extern bool enabled, audio;

    class Speedhack : public Hack
    {
    public:
        Speedhack();
        virtual void init() override;
        virtual void draw() override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
    };
}