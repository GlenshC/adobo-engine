#pragma once
#include "types.h"
#include "ggb/sparse.h"
#include "res/graphics/shader.h"
#include "binassets/binasset_read.h"

#define MAX_TEXTURES 12 
namespace texture
{
    const i32 INVALID_TEX_ID = -1;
    
    typedef i32 TextureID;
    typedef u32 TexGL;

    struct TextureRef
    {
        TexGL                 &id;
        adobo::vec2f          &tex_dim;
        adobo::vec4f         *&sub_tex;
        i32                   &sub_n;

        TextureRef &operator=(const TextureRef &other)
        {
            id      = other.id;
            tex_dim = other.tex_dim;
            sub_tex = other.sub_tex;
            sub_n   = other.sub_n;
            
            return  *this;
        }
    };

    struct Texture
    {
        TextureID id;

        
        inline TextureRef operator()();
        adobo::vec4f& operator[](i32 i)
        {
            return (*this)().sub_tex[i];
        }

        bool operator==(Texture &other)
        {
            return id == other.id;
        }
        bool operator!=(Texture &other)
        {
            return id != other.id;
        }
    };

    template<i32 MAX_T>
    struct TextureSoa {
        ggb::Sparse<MAX_T>   sparse;
        TexGL                tex_ids[MAX_T]       = {};
        adobo::vec2f         tex_dim[MAX_T]       = {}; // dimensions x, y
        adobo::vec4f        *sub_tex[MAX_T]       = {}; // should always contain something
        i32                  sub_n[MAX_T]         = {};
        i32                  max_capacity         = MAX_T;

        TextureRef operator[](i32 index)
        {
            return TextureRef{ 
                .id       = tex_ids[index],
                .tex_dim  = tex_dim[index],
                .sub_tex  = sub_tex[index],
                .sub_n    = sub_n[index],
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
    void        remove_tex2D(Texture &tex);

    // TODO: add xml sprite support
    // binary assets
    Texture loadAtlas2D(ggb::AssetAtlas &data, const u32 format_gl);
    Texture loadAtlas2D(ggb::AssetIMG   &data, const u32 format_gl, f32 tile_w, f32 tile_h);
    
    // (xml not supported, youll be using unordered map and i dont like that);
    // runtime assets 
    Texture loadAtlas2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h);
    
    Texture load2D(const char *path, const u32 format_gl);
    TexGL   load2D(u8 *imageData, const u32 format_gl, i32 x, i32 y);
    
    void    bindAtlas2D(Texture &tex, u32 shader_id, u32 slot);
    void    bindAtlas2D(Texture &tex, shader::Shader shader, u32 slot);
    
    void    bind2D(Texture &tex, u32 slot);
    void    bind2D(TexGL texgl, const u32 slot);

    void    set_onload_flipv(bool enabled);
    void    free_tex2D(TexGL *texgl);
}