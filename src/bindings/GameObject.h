#pragma once
#include "../pch.h"

namespace robtop
{
    inline const char* GameObject_objectID_pat = "8BF1E8????8B86^????3D????0F8F";
    inline uintptr_t GameObject_objectID_offset;

    class GameObject
    {
    public:
        inline uint32_t get_id()
        {
            if (!GameObject_objectID_offset)
                return 0;

            uintptr_t addr = (uintptr_t)this + GameObject_objectID_offset;
            return *(uint32_t *)addr;
        }
    };
}