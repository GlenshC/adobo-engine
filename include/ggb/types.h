#pragma once
#include <initializer_list>
namespace ggb 
{
    template<typename T, size_t N>
    struct vec 
    {
        T data[N];

        constexpr vec(std::initializer_list<T> init)
        {
            size_t i = 0;
            for (auto& v : init)
            {
                data[i++] = v;
            }
        }

        inline T&       operator[](size_t index) { return data[index]; }
        inline const T& operator[](size_t index) const { return data[index]; }
    };    

    template<typename T>
    struct vec<T, 2>
    {
        T x, y;

        constexpr vec(std::initializer_list<T> init)
        {
            auto it = init.begin();
            x = *it++;
            y = *it;
        }

        inline T&       operator[](size_t index) { return (index) ? y : x; }
        inline const T& operator[](size_t index) const { return (index) ? y : x; }
    };    
}