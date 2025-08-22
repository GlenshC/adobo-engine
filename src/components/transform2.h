#pragma once
#include "types.h"

namespace core 
{
    struct Xform2D 
    {
        adobo::vec2f position = {};
        adobo::vec2f scale = {1,1};
        adobo::vec3f rotation = {};
    };

    struct Xform2Dref
    {
        adobo::vec2f &position;
        adobo::vec2f &scale;
        adobo::vec3f &rotation;

        Xform2Dref& operator=(const Xform2Dref& other)
        {
            position = other.position;
            scale = other.scale;
            rotation = other.rotation;
            return *this;
        }

        Xform2Dref& operator=(const Xform2D& other)
        {
            position = other.position;
            scale = other.scale;
            rotation = other.rotation;
            return *this;
        }

        operator Xform2D() const 
        {
            return {.position = position, .scale = scale, .rotation = rotation};
        }
    };

    template<u32 MAX_CAP>
    struct Xform2Dsoa
    {
        adobo::vec2f position[MAX_CAP] = {};
        adobo::vec2f scale[MAX_CAP]    = {};
        adobo::vec3f rotation[MAX_CAP] = {};

        Xform2Dref operator[](size_t i)
        {
            return Xform2Dref{
                .position   = position[i],
                .scale      = scale[i],
                .rotation   = rotation[i]
            };
        }
    };
}