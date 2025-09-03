#pragma once
// #include <vector>
#include "types.h"
#include "renderer/shader.h"

#define MAX_TEX_SLOTS 16

namespace renderer
{
    // global variables
    extern const float g_quad_vertices[16];
    extern const uint32_t g_quad_indices[6];
    extern u32 g_vao_quad, g_vbo_quad, g_ibo_quad;
    
    void init(void);
    void draw_test_triangle();
    void init_test_triangle();
    void init_shape_renderer();
    void draw_shape_rect(adobo::vec4f &rect, const mat4 projection);
}
/*     
    // TODO: for complex 2D draws 
    struct RState_2D 
    {

    } 
*/