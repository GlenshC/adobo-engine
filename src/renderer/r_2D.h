#pragma once
#include "types.h"

namespace renderer 
{
    struct Xf2D 
    {
        adobo::vec2f position;
        adobo::vec2f scale;
        f32 rotation = 0;
    };

    struct Xf2Dref
    {
        adobo::vec2f &position;
        adobo::vec2f &scale;
        f32         &rotation;

        Xf2Dref& operator=(const Xf2D& other)
        {
            position = other.position;
            scale = other.scale;
            rotation = other.rotation;
            return *this;
        }

        operator Xf2D() const 
        {
            return {.position = position, .scale = scale, .rotation = rotation};
        }
    };
    
    template<u32 MAX_CAPACITY>
    struct Xf2Dsoa
    {
        adobo::vec2f position[MAX_CAPACITY];
        adobo::vec2f scale[MAX_CAPACITY];
        f32 rotation[MAX_CAPACITY];

        Xf2Dref operator[](size_t i)
        {
            return Xf2Dref{
                .position   = position[i],
                .scale      = scale[i],
                .rotation   = rotation[i]
            };
        }
    };
}