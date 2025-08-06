#pragma once
// #include <vector>
#include "types.h"
#include "resource/graphics/shader.h"

#define MAX_TEX_SLOTS 16

namespace renderer
{
    // global variables
    extern const float g_quad_vertices[16];
    extern const uint32_t g_quad_indices[6];
    extern u32 g_vao_quad, g_vbo_quad, g_ibo_quad;
    
    void init(void);
    void draw_test_triangle(shader::Shader &shader);
    void init_test_triangle();
}
/*     
    // TODO: for complex 2D draws 
    struct RState_2D 
    {

    } 
*/