#include <cstdio>
#include "util/mem_arena.h"
#include "util/string.h"

namespace util 
{
    char* string_readf(const char* path)
    {
        FILE *file = fopen(path, "r");
    
        fseek(file, 0, SEEK_END);
        unsigned long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        char *str = (char *)mem_alloc(size + 1);
        str[fread(str, sizeof(char), size, file)] = '\0';
    
        return str;
    }
}