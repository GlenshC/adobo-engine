#include "renderer/renderer.h"
#include <glad/gl.h>
#include "cglm/cglm.h"
#include "util/debug.h"

static shader::Shader s_test_triangle_shader;
static u32 s_test_triangle_vao;
static u32 s_test_triangle_vbo;

void rect2points_to_mat4(vec4 rect, mat4 dest);
static void rectxywh_to_mat4(vec4 rect, mat4 dest);

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
    shader::Shader g_shape_shader;
    u32 g_vao_quad, g_vbo_quad, g_ibo_quad;

    // run once
    void init(void)
    {
        glGenVertexArrays(1, &g_vao_quad);
        DEBUG_GLERR();

        glBindVertexArray(g_vao_quad);
        DEBUG_GLERR();
        // static quad buffer (shared)
        glGenBuffers(1, &g_vbo_quad);
        DEBUG_GLERR();

        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_quad);
        DEBUG_GLERR();

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(g_quad_vertices), g_quad_vertices, GL_STATIC_DRAW);
        DEBUG_GLERR();

        // a_pos (vec2)
        glVertexAttribPointer(
            0, 2, GL_FLOAT,
            GL_FALSE, 4 * sizeof(float), (void *)0);
        DEBUG_GLERR();

        glEnableVertexAttribArray(0);
        DEBUG_GLERR();

        // a_uv (vec2)
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), (void *)(2 * sizeof(float)));
        DEBUG_GLERR();

        glEnableVertexAttribArray(1);
        DEBUG_GLERR();

        // --- Index buffer ---
        glGenBuffers(1, &g_ibo_quad);
        DEBUG_GLERR();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo_quad);
        DEBUG_GLERR();

        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(g_quad_indices), g_quad_indices, GL_STATIC_DRAW);
        DEBUG_GLERR();

        glBindVertexArray(0);
        DEBUG_GLERR();

        DEBUG_LOG("Initialized main renderer.\n");
        init_shape_renderer();
    }


    // Temporary: draw a hardcoded triangle
    void draw_test_triangle()
    {
        glUseProgram(s_test_triangle_shader.id);
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

        shader::create(s_test_triangle_shader, "./assets/shader/test_triangle.vert", "./assets/shader/test_triangle.frag");

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

    void init_shape_renderer()
    {
        shader::create(g_shape_shader, "./assets/shader/sprite_hitbox.vert", "./assets/shader/sprite_hitbox.frag");
    }

    void draw_shape_rect(adobo::vec4f &rect, const mat4 projection)
    {
        static mat4 model;
        rectxywh_to_mat4(rect, model);

        shader::bind(g_shape_shader);
        glBindVertexArray(g_vao_quad);
        
        shader::set_uniform_mat4(g_shape_shader, "u_projection", projection);
        shader::set_uniform_mat4(g_shape_shader, "u_model", model);
        shader::set_uniform_vec4(g_shape_shader, "u_color", (vec4){1, 0, 0, 0.5f});
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

}

static void rectxywh_to_mat4(vec4 rect, mat4 dest)
{
    float x = rect[0], y = rect[1];
    float w = rect[2], h = rect[3];
    
    glm_mat4_identity(dest);

    glm_translate(dest, (vec3){x, y, 0.0f});

    glm_scale(dest, (vec3){w, h, 1.0f});
}

void rect2points_to_mat4(vec4 rect, mat4 dest)
{
    float x1 = rect[0], y1 = rect[1];
    float x2 = rect[2], y2 = rect[3];

    float w = x2 - x1;
    float h = y2 - y1;

    glm_mat4_identity(dest);

    // Translate to rect's origin (x1,y1)
    glm_translate(dest, (vec3){x1, y1, 0.0f});

    // Scale unit square to (w,h)
    glm_scale(dest, (vec3){w, h, 1.0f});
}
