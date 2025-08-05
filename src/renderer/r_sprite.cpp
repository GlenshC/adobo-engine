#include <glad/gl.h>
#include "renderer/r_sprite.h"
#include "resource/atlas.h"
// #include "shader.h"

#include "util/debug.h"
#include <iostream>
namespace renderer 
{
    static void reset_sprites(RState_Sprites& batch);

    void init_sprites(RState_Sprites& batch)
    {
        DEBUG_LOG("initialized renderer_sprites.\n");
        // TODO: consider mem arena for buffers
        glGenVertexArrays(1, &batch.vao);
        glBindVertexArray(batch.vao);

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
        
        glCreateBuffers(1, &batch.ssbo); // makes vbo buffers
        glNamedBufferStorage(batch.ssbo, sizeof(batch.data), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void begin_sprites(RState_Sprites& batch)
    {
        reset_sprites(batch);
    }

    void submit_sprites(RState_Sprites &batch, RSprite &sprite)
    {
        i32 tex_index = -1;
        i32 data_index = batch.sprite_count;
        u32 tex_id = sprite.atlas_dims->texture.id;

        
        if (batch.sprite_count >= MAX_SPRITES) 
        {
            end_sprites(batch);

            tex_index = batch.tex_count;
            
            batch.tex_slot[tex_index] = tex_id;
            batch.atlas[tex_index] = sprite.atlas_dims;
            batch.tex_count++;
        }
        else 
        {
            // FIX: might be buggy
            // find similar tex_id and set index
            for (i32 i = 0; i < batch.tex_count; i++)
            {
                if (batch.tex_slot[i] == tex_id) 
                {
                    tex_index = i;
                    break;
                }
            }
            // tex_index not found
            if (tex_index < 0)
            {
                // tex_slot overflow
                if (batch.tex_count >= MAX_TEX_SLOTS)
                {
                    end_sprites(batch);
                }
                tex_index = batch.tex_count;
                batch.atlas[tex_index] = sprite.atlas_dims;
                batch.tex_slot[tex_index] = tex_id;
                batch.tex_count++;
            }
        }

        batch.data[data_index].tex_id = tex_index;
        batch.data[data_index].uv_index = sprite.uv_index;
        batch.sprites_arr[data_index] = sprite.transform;
        batch.sprite_count++;
    }
    
    void end_sprites(RState_Sprites& batch)
    {
        flush_sprites(batch);
        draw_sprites(batch);
        reset_sprites(batch); // might remove
    }
    
    void flush_sprites(RState_Sprites& batch) 
    {
        for (i32 i = 0; i < batch.sprite_count; i++)
        {
            RData_S     &data   = batch.data[i];
            Xf2Dref sprite = batch.sprites_arr[i];
            glm_translate_to(
                GLM_MAT4_IDENTITY, 
                (vec3){sprite.position[0], sprite.position[1], 0},
                data.model
            );
            glm_rotate(data.model, glm_rad(sprite.rotation), (vec3){0.0f, 0.0f, 1.0f});
            glm_scale(data.model, (vec3){sprite.scale[0], sprite.scale[1], 0});
        }
        
        glNamedBufferSubData(batch.ssbo, 0, batch.sprite_count* sizeof(RData_S), batch.data);
    }
    
    void draw_sprites(RState_Sprites &batch)
    {
        for (i32 i = 0; i < batch.tex_count; i++)
        {
            atlas::bind(batch.shader_id, *batch.atlas[i], i);
        }
        glBindVertexArray(batch.vao);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.ssbo);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, batch.sprite_count);
    }

    static void reset_sprites(RState_Sprites& batch)
    {
        glBindVertexArray(0);
        batch.sprite_count = 0;
        batch.tex_count = 0;
    }
}