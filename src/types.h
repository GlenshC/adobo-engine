#pragma once

#include <cstdint>
#include <type_traits>
#include "ggb/types.h"

template <typename T>
constexpr T all_bits_one() {
    static_assert(std::is_integral_v<T>, "Integral type required");
    return static_cast<T>(~static_cast<T>(0));
}

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

namespace adobo
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
        
        operator f32*()
        {
            return (f32 *)this;
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

        operator f32*()
        {
            return (f32 *)this;
        }
        explicit operator vec2f*()
        {
            return ((vec2f*)this); 
        }
        explicit operator vec2f&()
        {
            return *((vec2f*)this); 
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
        
        operator f32*()
        {
            return (f32 *)this;
        }
        
        explicit operator vec3f*()
        {
            return ((vec3f*)this); 
        }
        explicit operator vec3f&()
        {
            return *((vec3f*)this); 
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
        
        operator f32*()
        {
            return (f32 *)this;
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
        
        operator f32*()
        {
            return (f32 *)this;
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
        
        operator f32*()
        {
            return (f32 *)this;
        }
    };
}