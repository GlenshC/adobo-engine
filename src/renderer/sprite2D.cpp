#include <glad/gl.h>
#include "renderer/sprite2D.h"
#include "core/entity/ecs.h"
#include "renderer/shader.h"

namespace renderer
{
    SpritesManager g_sprites;

    void init_SpritesManager(void)
    {
        DEBUG_LOG("Initializing renderer sprite2D...\n");
        glGenVertexArrays(1, &g_sprites.vao);
        DEBUG_GLERR();

        glBindVertexArray(g_sprites.vao);
        DEBUG_GLERR();
        
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_quad);
        DEBUG_GLERR();

        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, 
            4 * sizeof(float), (void *)0
        ); 
        DEBUG_GLERR();

        glEnableVertexAttribArray(0); 
        DEBUG_GLERR();
        
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 
            4 * sizeof(float), (void *)(2 * sizeof(float))
        ); 
        DEBUG_GLERR();

        glEnableVertexAttribArray(1); 
        DEBUG_GLERR();
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo_quad); 
        DEBUG_GLERR();
        
        glCreateBuffers(1, &g_sprites.ssbo); 
        DEBUG_GLERR(); // makes vbo buffers

        glNamedBufferStorage(g_sprites.ssbo, sizeof(RDataSprite) * ADOBO_ENGINE_MAX_SPRITES, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT); 
        DEBUG_GLERR();

        glBindVertexArray(0); 
        DEBUG_GLERR();

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        DEBUG_GLERR();
        
        g_sprites.init(ADOBO_ENGINE_MIN_SPRITES);
        DEBUG_LOG("Initalized renderer sprite2D.\n");
    }

    void submit_sprites(ecs::Entity2D &ent)
    {
        i32 tex_index = -1;
        i32 data_index = (i32)g_sprites.size;

        ecs::Entity2Dref entity = ecs::g_entities(ent);
        texture::Texture &tex = entity.tex;

        if (g_sprites.size + ADOBO_ENGINE_REALLOC_MARGIN >= g_sprites.capacity)
        {
            g_sprites.reserve(g_sprites.capacity << 1);
        }
            // texture handling
            // tex_slot must contain texgl_id
            // data.tex_id must contain index to tex_slot where the texgl_id is

        for (size_t i = 0; i < g_sprites.tex_count; i++)
        {
            if (g_sprites.tex_slot[i] == tex)
            {
                tex_index = i;
                break;
            }
        }
        // tex_index not found
        if (tex_index < 0)
        {
            // tex_slot overflow
            if (g_sprites.tex_count >= MAX_TEX_SLOTS)
            {
                flush_sprites();
                render_sprites();
                clear_sprites();
            }
            tex_index = g_sprites.tex_count++;
            g_sprites.tex_slot[tex_index] = tex;
        }

        g_sprites.data[data_index].tex_uv    = entity.tex_uv;
        g_sprites.data[data_index].tex_index = tex_index;
        g_sprites.position[data_index]       = entity.position;
        g_sprites.scale[data_index]          = entity.scale;
        g_sprites.rotation[data_index]       = entity.rotation;

        g_sprites.size++;
    }

    void clear_sprites(void)
    {
        g_sprites.size = 0;
        g_sprites.tex_count = 0;
    }

    void begin_sprites(shader::Shader shader)
    {
        g_sprites.shader_id = shader.id; 
        clear_sprites();
    }

    void end_sprites(void)
    {
        flush_sprites();
        render_sprites();
    }

    void flush_sprites(void)
    {
        for (size_t i = 0; i < g_sprites.size; i++)
        {
            mat4 rot;
            RDataSprite &data       = g_sprites.data[i];
           
            glm_translate_to(
                GLM_MAT4_IDENTITY, 
                g_sprites.position[i],
                data.model
            );
            glm_euler_xyz(g_sprites.rotation[i], rot);
            glm_mat4_mul(data.model, rot, data.model);
            glm_scale(data.model, (vec3){g_sprites.scale[i].x, g_sprites.scale[i].y, 0});
        }
        
        glNamedBufferSubData(g_sprites.ssbo, 0, g_sprites.size * sizeof(RDataSprite), g_sprites.data);
        DEBUG_GLERR();
    }

    void render_sprites(void)
    {
        shader::bind(g_sprites.shader_id);

        for (size_t i = 0; i < g_sprites.tex_count; i++)
        {
            // DEBUG_LOG("bind %zu\n", i);
            texture::bindAtlas2D(g_sprites.tex_slot[i], g_sprites.shader_id, i);
        }
        glBindVertexArray(g_sprites.vao);
        DEBUG_GLERR();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_sprites.ssbo);
        DEBUG_GLERR();
        
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, g_sprites.size);
        DEBUG_GLERR();
        // DEBUG_LOG("render\n");
    }
}