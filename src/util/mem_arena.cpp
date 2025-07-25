#include <cstdlib>

namespace util 
{
    void *mem_alloc(size_t size)
    {
        return malloc(size);
    }

    void mem_free(void *mem)
    {
        free(mem);
    }
}