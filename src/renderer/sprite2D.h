#pragma once

#include "ggb/cglm.h"
#include "types.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "core/entity/ecs.h"

#include "core/constants.h"

namespace renderer
{
    /* TYPES */
    typedef u16 TexGL16;
    struct RDataSprite;
    struct SpriteRef;
    struct SpritesManager;

    /* GLOBALS */
    extern SpritesManager g_sprites;

    /* FUNCTIONS */
    template<i32 N>
    inline void submit_sprites(ecs::Entity2DGroup<N> &ent);

    void begin_sprites(shader::Shader shader);
    void submit_sprites(ecs::Entity2D &ent);
    void end_sprites(void);
    
    void init_SpritesManager(void);
    void flush_sprites(void);
    void render_sprites(void);
    void clear_sprites(void);

    /* TYPE DEFS */
    struct RDataSprite
    {
        mat4            model     = GLM_MAT4_IDENTITY_INIT;
        adobo::vec4f    tex_uv    = {};
        texture::TexGL  tex_index = 0;
    };

    struct SpriteRef
    {
        adobo::vec2f     &position;
        adobo::vec2f     &scale;
        adobo::vec3f     &rotation;
        adobo::vec4f     &tex_uv; 
        texture::TexGL   &tex_index;

        SpriteRef &operator=(SpriteRef &other)
        {
            position   = other.position;
            scale      = other.scale;
            rotation   = other.rotation;
            tex_uv     = other.tex_uv;
            tex_index  = other.tex_index;
            return *this;
        }
    };

    struct SpritesManager
    {
        char             *_bp;
        RDataSprite      *data;
        adobo::vec2f     *position;
        adobo::vec2f     *scale;
        adobo::vec3f     *rotation;
        texture::Texture tex_slot[MAX_TEX_SLOTS];
        
        size_t  capacity;
        size_t  tex_count;
        size_t  size;
        
        u32     vao;
        u32     ssbo;
        u32     shader_id;

        void init(size_t n);
        i32  reserve(size_t new_cap);
        void clear(void);

        SpriteRef operator[](size_t index);
    };

    /* SpritesManager */
    inline void SpritesManager::init(size_t n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*data)     * n +
                sizeof(*position) * n +
                sizeof(*scale)    * n + 
                sizeof(*rotation) * n
            );
            if (!mem)
            {
                DEBUG_ERR("Sparse: BAD ALLOCATION\n");
                return;
            }
            size_t offset = 0;
            _bp      = mem;
            data     = (RDataSprite  *)     (mem);
            position = (adobo::vec2f *)     (mem + (offset += n * sizeof(*data)));
            scale    = (adobo::vec2f *)     (mem + (offset += n * sizeof(*position)));
            rotation = (adobo::vec3f *)     (mem + (offset += n * sizeof(*scale)));
            
            size = 0;
            capacity = n;
            DEBUG_LOG("Initialized SpritesManager(%zu).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to init SpritesManager.\n");
    }

    inline i32 SpritesManager::reserve(size_t new_cap)
    {
        if (new_cap <= capacity) return 1;

        char *mem = (char *)std::realloc(
            _bp,
            sizeof(*data)     * new_cap +
            sizeof(*position) * new_cap +
            sizeof(*scale)    * new_cap + 
            sizeof(*rotation) * new_cap
        );
        if (!mem)
        {
            DEBUG_ERR("Entity2DManager: Resize Error\n");
            return 1;
        }
        size_t boffset = 
            sizeof(*data)     * capacity +
            sizeof(*position) * capacity +
            sizeof(*scale)    * capacity; 

        size_t offset = 0;
        _bp = mem;
        data        = (RDataSprite  *)     (mem);
        position    = (adobo::vec2f *)     (mem + (offset += sizeof(*data)     * new_cap)); 
        scale       = (adobo::vec2f *)     (mem + (offset += sizeof(*position) * new_cap));
        rotation    = (adobo::vec3f *)     (mem + (offset += sizeof(*scale)    * new_cap));

        if (new_cap >= (capacity << 1)) // memcpy
        {
            memcpy(rotation, (_bp + (boffset)), sizeof(*rotation) * capacity);
            memcpy(scale   , (_bp + (boffset -= sizeof(*scale)    * capacity)), sizeof(*scale)    * capacity);
            memcpy(position, (_bp + (boffset -= sizeof(*position) * capacity)), sizeof(*position) * capacity);
        }
        else
        {
            memmove(rotation, (_bp + (boffset)), sizeof(*rotation) * capacity);
            memmove(scale   , (_bp + (boffset -= sizeof(*scale)    * capacity)), sizeof(*scale)    * capacity);
            memmove(position, (_bp + (boffset -= sizeof(*position) * capacity)), sizeof(*position) * capacity);
        }
        capacity = new_cap;
        DEBUG_LOG("Resized SpritesManager(%zu).\n", capacity);
        return 0; 
    }

    inline SpriteRef SpritesManager::operator[](size_t index)
    {
        return SpriteRef{
            position[index],
            scale[index],
            rotation[index],
            data[index].tex_uv,
            data[index].tex_index,
        };
    }

    inline void SpritesManager::clear(void)
    {
        tex_count = 0;
        size = 0;
    }

    /* FUNCTION DEFS */
    template<i32 N>
    inline void submit_sprites(ecs::Entity2DGroup<N> &ent)
    {
        for (int i=0; i < N; i++)
            submit_sprites(ent.data[i]);
    }
}