#pragma once
#include "types.h"

namespace util 
{
    void *mem_alloc(size_t size);
    void mem_free(void *mem);
}
