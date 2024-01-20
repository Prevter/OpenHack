#pragma once
#include "../pch.h"

namespace robtop
{
    class GameObject
    {
    public:
        inline uint32_t get_id()
        {
            uint32_t id = ((uint32_t*)this)[0xE1];
            return id;
        }
    };
}