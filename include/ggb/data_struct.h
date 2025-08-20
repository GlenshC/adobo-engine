#pragma once 

#define GGB_SPARSE_SWAP_REMOVE
#define GGB_DS_PUSH_NOTHING
#include <algorithm>
namespace ggb 
{

    template <typename T, size_t N>
    struct SparseArr
    {
        T data[N] = {};
        size_t size = 0;
        const size_t capacity = N;

        void remove(const size_t i)
        {
            if (size <= 0) return;
            #ifdef GGB_SPARSE_SWAP_REMOVE
            std::swap(data[i],data[--size]);
            #else
            data[i] = data[--size];
            #endif
        }

        T& push(const T &n)
        {
            if (size >= capacity) return data[0];
            return data[size++] = n;
        }
        T& push(T && n)
        {
            if (size >= capacity) return data[0];
            return data[size++] = std::move(n);
        }
        T& push(void)
        {
            if (size >= capacity) return data[0];
            return data[size++];
        }

        T& operator[](const size_t i)
        {
            return data[i];
        }

        const T& operator[](const size_t i) const
        {
            return data[i];
        }
    };



    template <typename T, int N>
    struct RingQueue
    {
        T data[N] = {};
        int size = 0;
        int f_index = 0;
        int b_index = 0;
        const int capacity = N;

        T &back()
        {
            return data[b_index];
        }

        T &pop(void)
        {
            if (size <= 0)
            {
                return data[0];
            }
            size--;
            auto &ret = data[f_index];
            f_index = ++f_index % N;
            return ret;
        }

        T &push(T n)
        {
            if (size > capacity)
            {
                return data[0];
            }
            size++;
            auto &ret = data[b_index] = n;
            b_index = (++b_index) % N;
            return ret;
        }

        #ifdef GGB_DS_PUSH_NOTHING
        T &push(void)
        {
            if (size > capacity)
            {
                return data[0];
            }
            size++;
            auto &ret = data[b_index];
            b_index = (++b_index) % N;
            return ret;
        }
        #endif

        const T &operator[](size_t i) const
        {
            return data[i];
        }

        T &operator[](size_t i)
        {
            return data[i];
        }
    };
}

