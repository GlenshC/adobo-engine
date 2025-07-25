#pragma once
#include "types.h"

namespace metrix 
{
    // store the score in separate struct to not destroy struct alignment
    template<i32 ROW, i32 COL>
    struct MetrixData
    {
        
        u8 data[ROW][COL];
    };
    
    struct AtomBlock
    {
        
        ggb::vec<u8, 2> pos;
        u8 type;
    };
    
    struct Block {
        AtomBlock blocks[]; 
    };

    extern MetrixData<20, 10> g_matrix;
}