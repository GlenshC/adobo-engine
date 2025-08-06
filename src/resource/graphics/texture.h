#pragma once
#include "types.h"
#include "ggb/sparse.h"
#include "resource/graphics/shader.h"

#define MAX_TEXTURES 200
namespace texture
{
    const i32 INVALID_TEX_ID = -1;
    
    typedef i32 Texture;
    typedef u32 TexGL;

    struct TextureRef
    {
        u32          &id;
        adobo::vec4f &tex_dim;

        TextureRef &operator=(TextureRef &other)
        {
            id      = other.id;
            tex_dim = other.tex_dim;
            
            return  *this;
        }
    };

    template<i32 MAX_T>
    struct TextureSoa
    {
        /* members */
        TexGL        tex_ids[MAX_T]  = {};
        adobo::vec4f tex_dim[MAX_T]  = {}; 
        ggb::Sparse<MAX_T> sparse;
        const i32    capacity        = MAX_T;

        /* operators */
        // indexing
        TextureRef operator[](i32 index)
        {
            return TextureRef{ 
                .id       = tex_ids[index],
                .tex_dim  = tex_dim[index],
            }; 
        }

        // convert id to index and indexing
        TextureRef operator()(Texture tex_id)
        {
            i32 index = sparse[tex_id];
            return TextureRef{ 
                .id       = tex_ids[index],
                .tex_dim  = tex_dim[index],
            }; 
        }
        
        /* functions */
        i32 get_index(Texture id)
        {
            return sparse[id];
        }

        Texture push(TexGL tex_gl, f32 tex_w, f32 tex_h, f32 tile_w, f32 tile_h) 
        {
            i32 index;
            Texture id = sparse.create_id(index); // this increments size

            tex_ids[index] = tex_gl;
            tex_dim[index] = (adobo::vec4f){tex_w, tex_h, tile_w, tile_h};      
            
            return id;
        }
        
        Texture push(TexGL tex_gl, f32 tex_w, f32 tex_h)
        {
            return push(tex_gl, tex_w, tex_h, tex_w, tex_h);
        } 

        void remove(Texture &id)
        {
            i32 index      = sparse.remove(id);
            i32 last_index = sparse.size;
            this[index] = this[last_index];

            id = INVALID_TEX_ID;
        }
    };

    extern TextureSoa<MAX_TEXTURES> g_textures;

    Texture loadAtlas2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h);
    Texture load2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h);
    Texture load2D(const char *path, const u32 format_gl);
    
    void    bindAtlas2D(u32 shader_id, Texture tex_id, u32 slot);
    void    bindAtlas2D(shader::Shader shader, Texture tex_id, u32 slot);
    void    bind2D(Texture tex_id, u32 slot);

    void    set_onload_flipv(bool enabled);
    void    free(Texture &tex_id);
}