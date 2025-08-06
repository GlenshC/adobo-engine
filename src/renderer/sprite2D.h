#pragma once

#include "ggb/cglm.h"
#include "types.h"
#include "renderer/renderer.h"
#include "components/transform.h"
#include "resource/graphics/texture.h"
#include "core/entity/ecs.h"

#define MAX_SPRITES 200


namespace renderer
{
    struct RDataSprite
    {
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        u16  uv_index;
        u16  tex_id = 0;
        u32  pad[3]; // temp 
    };

    struct SpriteRef
    {
        core::Xform2Dref transform;
        u16              &curr_frame; // internally this is uv_index
        u16              &tex_id;

        SpriteRef &operator=(SpriteRef &other)
        {
            transform  = other.transform;
            curr_frame = other.curr_frame;
            tex_id     = other.tex_id;
            return *this;
        }
    };


    template<i32 MAX_S>
    struct SpritesSoa
    {
        u32         vao = 0, ssbo = 0;
        u32         shader_id = {};

        RDataSprite             data[MAX_SPRITES];
        texture::Texture        tex_slot[MAX_TEX_SLOTS] = {};
        core::Xform2Dsoa<MAX_S> sprites;

        i32 tex_count      = 0;
        i32 sprite_count   = 0;
        const i32 capacity = MAX_S;

        SpriteRef operator[](i32 index)
        {
            return SpriteRef{
                .transform  = sprites[index],
                .curr_frame = data[index].uv_index,
                .tex_id     = data[index].tex_id,
            };
        }

        void clear(void)
        {
            tex_count = 0;
            sprite_count = 0;
        }
    };

    extern SpritesSoa<MAX_SPRITES> g_sprites;

    void begin_sprites(shader::Shader shader);
    void submit_sprites(ecs::Entity2D ent_id);
    void end_sprites(void);
    
    void init_sprites(void);
    void flush_sprites(void);
    void render_sprites(void);
    void clear_sprites(void);
}