#pragma once

#include "ggb/cglm.h"
#include "types.h"
#include "renderer/renderer.h"
#include "components/transform.h"
#include "res/graphics/texture.h"
#include "core/entity/ecs.h"

#define MAX_SPRITES 200


namespace renderer
{
    typedef u16 TexGL16;
    struct RDataSprite
    {
        mat4            model     = GLM_MAT4_IDENTITY_INIT;
        adobo::vec4f    tex_uv    = {};
        texture::TexGL  tex_index = 0;
    };

    struct SpriteRef
    {
        core::Xform2Dref transform;
        adobo::vec4f     &tex_uv; // internally this is uv_index
        texture::TexGL   &tex_index;

        SpriteRef &operator=(SpriteRef &other)
        {
            transform  = other.transform;
            tex_uv     = other.tex_uv;
            tex_index  = other.tex_index;
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
                .tex_uv     = data[index].tex_uv,
                .tex_index  = data[index].tex_index,
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
    void submit_sprites(ecs::Entity2D &ent);
    void end_sprites(void);
    
    void init_sprites(void);
    void flush_sprites(void);
    void render_sprites(void);
    void clear_sprites(void);
}