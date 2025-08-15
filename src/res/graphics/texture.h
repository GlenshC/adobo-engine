#pragma once
#include "types.h"
#include "ggb/sparse.h"
#include "res/graphics/shader.h"
#include "imgbin.h"

#define MAX_TEXTURES 200
namespace texture
{
    const i32 INVALID_TEX_ID = -1;
    
    typedef i32 TextureID;
    typedef u32 TexGL;

    struct TextureRef
    {
        TexGL        &id;
        adobo::vec4f &tex_dim;

        TextureRef &operator=(const TextureRef &other)
        {
            id      = other.id;
            tex_dim = other.tex_dim;
            
            return  *this;
        }
    };

    struct Texture
    {
        TextureID id;

        inline TextureRef operator()();
        bool operator==(Texture &other)
        {
            return id == other.id;
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
        TextureRef operator()(TextureID tex_id)
        {
            return (*this)[sparse[tex_id]];
        }
        
        TextureRef operator()(Texture &tex)
        {
            return (*this)(tex.id);
        }
    };

    extern TextureSoa<MAX_TEXTURES> g_textures;

    inline TextureRef Texture::operator()()
    {
        return g_textures(Texture::id);
    }

    TextureRef  create_tex2D(Texture &tex_out);
    Texture     create_tex2D();
    void        remove_tex2D(Texture &tex);

    Texture loadAtlas2D(ggb::AssetAtlas &data, const u32 format_gl);
    Texture loadAtlas2D(ggb::AssetIMG &data, const u32 format_gl, f32 tile_w, f32 tile_h);
    Texture loadAtlas2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h);
    
    Texture load2D(u8 *imageData, const u32 format_gl, f32 tile_w, f32 tile_h, i32 x, i32 y, i32 channels);
    Texture load2D(ggb::AssetIMG &data, const u32 format_gl, f32 tile_w, f32 tile_h);
    Texture load2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h);
    Texture load2D(const char *path, const u32 format_gl);
    
    void    bindAtlas2D(u32 shader_id, Texture &tex, u32 slot);
    void    bindAtlas2D(shader::Shader shader, Texture &tex, u32 slot);
    void    bind2D(Texture &tex, u32 slot);
    void    bind2D(TexGL texgl, const u32 slot);

    void    set_onload_flipv(bool enabled);
    void    free_tex2D(TexGL *texgl);
}