#include <glad/gl.h>
#include "renderer/sprite2D.h"
#include "core/entity/ecs.h"
#include "renderer/shader.h"

namespace renderer
{
    SpritesSoa<MAX_SPRITES> g_sprites;

    void init_sprites(void)
    {
        DEBUG_LOG("initialized renderer_sprites.\n");
        // TODO: consider mem arena for buffers
        glGenVertexArrays(1, &g_sprites.vao);
        glBindVertexArray(g_sprites.vao);

        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_quad);
        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, 
            4 * sizeof(float), (void *)0
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 
            4 * sizeof(float), (void *)(2 * sizeof(float))
        );
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo_quad);
        
        glCreateBuffers(1, &g_sprites.ssbo); // makes vbo buffers
        glNamedBufferStorage(g_sprites.ssbo, sizeof(g_sprites.data), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void submit_sprites(ecs::Entity2D &ent)
    {
        i32 tex_index = -1;
        i32 data_index = g_sprites.sprite_count;

        ecs::Entity2Dref entity = ecs::g_entities(ent);
        texture::Texture &tex = entity.tex;

        if (g_sprites.sprite_count >= g_sprites.capacity)
        {
            flush_sprites();
            render_sprites();
            clear_sprites();
        }
        else
        {
            // texture handling
            // tex_slot must contain texgl_id
            // data.tex_id must contain index to tex_slot where the texgl_id is

            for (i32 i = 0; i < g_sprites.tex_count; i++)
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
        }
        
        g_sprites[data_index].tex_uv     = entity.tex_uv;
        g_sprites[data_index].tex_index  = tex_index;
        g_sprites[data_index].transform.position = entity.position;
        g_sprites[data_index].transform  = entity.position;
        g_sprites[data_index].transform  = entity.position;
        g_sprites.sprite_count++;
    }

    void clear_sprites(void)
    {
        g_sprites.sprite_count = 0;
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
        for (i32 i = 0; i < g_sprites.sprite_count; i++)
        {
            mat4 rot;
            RDataSprite &data       = g_sprites.data[i];
            core::Xform2Dref sprite = g_sprites.sprites[i];
            glm_translate_to(
                GLM_MAT4_IDENTITY, 
                (vec3){sprite.position[0], sprite.position[1], 0},
                data.model
            );
            glm_euler_xyz(sprite.rotation, rot);
            glm_mat4_mul(data.model, rot, data.model);
            glm_scale(data.model, (vec3){sprite.scale[0], sprite.scale[1], 0});
        }
        
        glNamedBufferSubData(g_sprites.ssbo, 0, g_sprites.sprite_count* sizeof(RDataSprite), g_sprites.data);
    }

    void render_sprites(void)
    {
        shader::bind(g_sprites.shader_id);

        for (i32 i = 0; i < g_sprites.tex_count; i++)
        {
            texture::bindAtlas2D(g_sprites.tex_slot[i], g_sprites.shader_id, i);
        }
        glBindVertexArray(g_sprites.vao);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_sprites.ssbo);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, g_sprites.sprite_count);
    }
}