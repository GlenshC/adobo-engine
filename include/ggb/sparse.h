#pragma once
#include "util/debug.h"
#include <cstdlib>
#include <limits>

#define INVALID_ID std::numeric_limits<T>::max();
namespace ggb
{
    template<typename T>
    struct Sparse;

    template<typename T>
    void free_sparse(Sparse<T> data);


    template<typename T>
    struct Sparse
    {
        typedef T SparseID, DenseKey;

        size_t    capacity;
        size_t    size;
        size_t    free_size;
        DenseKey *id_to_index; // (this sparse) keys to access the dense
        SparseID *index_to_id; // keys to access the sparse
        SparseID *free_id;     // to access the sparse
        char     *_bp;

        void            init(size_t n);
        int             reserve(size_t new_cap);
        SparseID        create_id(void);
        SparseID        create_id(DenseKey &index_out);
        DenseKey        remove(SparseID id);
        DenseKey&       operator[](SparseID id);
        const DenseKey& operator[](SparseID id) const;
        constexpr T invalid_id() const;
    };

    template <typename T>
    inline constexpr T Sparse<T>::invalid_id() const
    {
        return std::numeric_limits<T>::max();
    }

    template <typename T>
    inline void Sparse<T>::init(size_t n)
    {
        if (n > 0)
        {
            const size_t size_m = sizeof(T) * n;
            _bp = (char *)std::malloc(size_m * 3);
            if (!_bp)
            {
                DEBUG_ERR("Sparse: BAD ALLOCATION\n");
                return;
            }
            size_t offset = 0;
            
            id_to_index = (T *)(_bp);
            index_to_id = (T *)(_bp + (offset += size_m));
            free_id     = (T *)(_bp + (offset += size_m));
            
            size = 0;
            capacity = n;
            return;
        }
        DEBUG_ERR("failed to init Sparse\n");
    }

    template <typename T>
    inline int Sparse<T>::reserve(size_t new_cap)
    {
        if (new_cap <= capacity) return 1;

        char *mem = (char *)std::realloc(_bp, new_cap * sizeof(T) * 3);
        if (!mem)
        {
            DEBUG_ERR("Sparse: Resize Error\n");
            return 1;
        }

        _bp = mem;
        id_to_index = (T *)(mem);
        index_to_id = (T *)(mem + new_cap * sizeof(T));
        free_id     = (T *)(mem + new_cap * sizeof(T) * 2);

        size_t size_m = sizeof(T) * capacity;
        if (new_cap >= (capacity << 1)) // memcpy
        {
            memcpy(free_id, (_bp + size_m * 2), size_m);
            memcpy(index_to_id, (_bp + size_m), size_m);
        }
        else
        {
            memmove(free_id, (_bp + size_m * 2), size_m);
            memmove(index_to_id, (_bp + size_m), size_m);
        }
        capacity = new_cap;
        return 0;
    }

    // operators
    template <typename T>
    inline typename Sparse<T>::DenseKey &Sparse<T>::operator[](SparseID id)
    {
#ifdef DEBUG_ENABLED
        if (id >= (T)capacity || id == this->invalid_id())
        {
            DEBUG_LOG("invalid id");
        }
#endif
        return id_to_index[id];
    }

    template <typename T>
    inline const typename Sparse<T>::DenseKey &Sparse<T>::operator[](SparseID id) const
    {
#ifdef DEBUG_ENABLED
        if (id >= (T)capacity || id == this->invalid_id())
        {
            DEBUG_LOG("Sparse: invalid id\n");
        }
#endif
        return id_to_index[id];
    }

    template <typename T>
    inline typename Sparse<T>::SparseID Sparse<T>::create_id(DenseKey &index_out)
    {
        if (size >= capacity)
        {
            if (reserve(capacity << 1))
                return INVALID_ID;
        }

        DenseKey index = size++;
        SparseID id = index;

        if (free_size)
        {
            id = free_id[--free_size];
        }
        index_to_id[index] = id;
        id_to_index[id] = index;

        index_out = index;

        return id;
    }

    template <typename T>
    inline typename Sparse<T>::SparseID Sparse<T>::create_id(void)
    {
        if (size >= capacity)
        {
            if (reserve(capacity << 1)) return INVALID_ID;
        }

        DenseKey index = size++;
        SparseID id = index;

        if (free_size)
        {
            id = free_id[--free_size];
        }
        index_to_id[index] = id;
        id_to_index[id] = index;

        return id;
    }

    // returns the index of the element to be replaced with the last element
    template <typename T>
    inline typename Sparse<T>::DenseKey Sparse<T>::remove(SparseID id)
    {
        if (size <= 0 || id >= (T)capacity || id == this->invalid_id())
            return this->invalid_id();

        DenseKey index = id_to_index[id];
        DenseKey last_index = --size;

        id_to_index[index_to_id[last_index]] = index;
        return index;
    }

    template<typename T>
    inline void free_sparse(Sparse<T> data)
    {
        if (!data._bp)
        {
            DEBUG_LOG("free_sparse: Error freeing nullptr.\n");
            return;
        }
        free(data._bp);
        data._bp         = nullptr;
        data.free_id     = nullptr;
        data.id_to_index = nullptr;
        data.index_to_id = nullptr;
        data.capacity    = 0;
        data.size        = 0;
    }
}