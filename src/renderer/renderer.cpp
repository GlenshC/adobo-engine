#include "renderer/renderer.h"
#include "util/debug.h"
#include <glad/gl.h>

static u32 s_test_triangle_vao;
static u32 s_test_triangle_vbo;

namespace renderer
{
    const float g_quad_vertices[16] = {
        //  x      y     u     v
        -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f, 1.0f, 1.0f,   // Top-right
        -0.5f,  0.5f, 0.0f, 1.0f   // Top-left
    };

    const uint32_t g_quad_indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    // constant quad
    u32 g_vao_quad, g_vbo_quad, g_ibo_quad;

    // run once
    void init(void)
    {
        glGenVertexArrays(1, &g_vao_quad);
        glBindVertexArray(g_vbo_quad);
        // static quad buffer (shared)
        glGenBuffers(1, &g_vbo_quad);
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_quad);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(g_quad_vertices), g_quad_vertices, GL_STATIC_DRAW);

        // a_pos (vec2)
        glVertexAttribPointer(
            0, 2, GL_FLOAT,
            GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // a_uv (vec2)
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // --- Index buffer ---
        glGenBuffers(1, &g_ibo_quad);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo_quad);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(g_quad_indices), g_quad_indices, GL_STATIC_DRAW);
        glBindVertexArray(0);
        DEBUG_LOG("Initialized main renderer.\n");
    }


    // Temporary: draw a hardcoded triangle
    void draw_test_triangle(shader::Shader &shader)
    {
        glUseProgram(shader.id);
        glBindVertexArray(s_test_triangle_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void init_test_triangle()
    {
        static const float triangle_vertices[] = {
            // Position        // Color
            0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // Top (Red)
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Left (Green)
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // Right (Blue)
        };

        glGenVertexArrays(1, &s_test_triangle_vao);
        glBindVertexArray(s_test_triangle_vao);

        glGenBuffers(1, &s_test_triangle_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, s_test_triangle_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3));
    }

}
