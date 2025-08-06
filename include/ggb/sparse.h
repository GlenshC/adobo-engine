#pragma once
#include "util/debug.h"

namespace ggb
{
    const int INVALID_ID = -1;

    typedef int SparseID;
    typedef int DenseKey;

    template<int MAX_CAP>
    struct Sparse
    {
        // -1 will be invalid;
        DenseKey    id_to_index[MAX_CAP] = {}; // (this sparse) keys to access the dense
        SparseID    index_to_id[MAX_CAP] = {}; // keys to access the sparse
        SparseID    free_id[MAX_CAP]     = {}; // to access the sparse

        int         free_size            = 0;
        int         size                 = 0;
        const int   capacity             = MAX_CAP;

        // operators
        DenseKey& operator[](SparseID id)
        {
            #ifdef DEBUG_ENABLED
            if (id < 0 || id >= capacity)
            {
                DEBUG_LOG("invalid id");
            }
            #endif
            return id_to_index[id];
        }

        const DenseKey& operator[](SparseID id) const
        {
            return id_to_index[id];
        }

        SparseID create_id(DenseKey &index_out)
        {
            if (size >= capacity)
            {
                DEBUG_LOG("Sparse FULL"); // debug
                index_out = INVALID_ID;
                return INVALID_ID;
            }

            DenseKey index = size++;
            SparseID id    = index;

            if (free_size)
            {
                id = free_id[--free_size];
            }
            index_to_id[index] = id;
            id_to_index[id]    = index;
            
            index_out = index;

            return id;
        }

        SparseID create_id(void)
        {
            if (size >= capacity)
            {
                DEBUG_LOG("Sparse FULL"); // debug
                return INVALID_ID;
            }

            DenseKey index = size++;
            SparseID id    = index;

            if (free_size)
            {
                id = free_id[--free_size];
            }
            index_to_id[index] = id;
            id_to_index[id]    = index;

            return id;
        }

        DenseKey remove(SparseID id)
        {
            DenseKey index      = id_to_index[id];
            DenseKey last_index = --size;
            // data[index]      = data[last_index];
            id_to_index[index_to_id[last_index]] = index;
            return index;
        }

    };
}