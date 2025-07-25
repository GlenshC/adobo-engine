#pragma once

#include <cstdint>
#include "ggb/types.h"

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

namespace mtrx
{
    struct vec2f
    {
        f32 x = 0, y = 0;

        f32& operator[](size_t i)
        {
            return ((f32 *)this)[i];
        }
        const f32& operator[](size_t i) const
        {
            return ((f32 *)this)[i];
        }
    };

    struct vec3f
    {
        f32 x = 0, y = 0, z = 0;

        f32& operator[](size_t i)
        {
            return ((f32 *)this)[i];
        }
        const f32& operator[](size_t i) const
        {
            return ((f32 *)this)[i];
        }
    };
    struct vec4f
    {
        f32 x = 0, y = 0, z = 0, w = 0;

        f32& operator[](size_t i)
        {
            return ((f32 *)this)[i];
        }
        const f32& operator[](size_t i) const
        {
            return ((f32 *)this)[i];
        }
    };

    //templated
    template<typename T>
    struct vec2
    {
        T x = 0, y = 0;

        T& operator[](size_t i)
        {
            return ((T *)this)[i];
        }
        const T& operator[](size_t i) const
        {
            return ((T *)this)[i];
        }
    };

    template<typename T>
    struct vec3
    {
        T x = 0, y = 0, z = 0;    
        
        T& operator[](size_t i)
        {
            return ((T *)this)[i];
        }
        const T& operator[](size_t i) const
        {
            return ((T *)this)[i];
        }
    };

    template<typename T>
    struct vec4
    {
        T x = 0, y = 0, z = 0, w = 0;

        T& operator[](size_t i)
        {
            return ((T *)this)[i];
        }
        const T& operator[](size_t i) const
        {
            return ((T *)this)[i];
        }
    };
}