#pragma once
#include "types.h"
#include "ggb/sparse.h"
#include "renderer/shader.h"
#include "binassets/binasset_read_decl.h"

#define MAX_TEXTURES 12 
namespace texture
{
    /* CONST */
    const i32 INVALID_TEX_ID = -1;
    

    /* TYPES */
    typedef i32 TextureID;
    typedef u32 TexGL;
    
    struct TextureRef;
    struct Texture;
    template<i32 MAX_T>
    struct TextureSoa;

    
    /* GLOBALS */
    extern TextureSoa<MAX_TEXTURES> g_textures;


    /* FUNCTIONS */
    TextureRef  create_tex2D(Texture &tex_out);
    void        remove_tex2D(Texture &tex);

    // TODO: add xml sprite support
    // (xml not supported, youll be using unordered map and i dont like that);
    Texture loadAtlas2D(binassets::AssetAtlas &atlas, const u32 format_gl);
    Texture loadAtlas2D(binassets::AssetIMG   &img, const u32 format_gl, f32 tile_w, f32 tile_h);
    Texture loadAtlas2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h);
    
    Texture load2D(const char *path, const u32 format_gl);
    TexGL   load2D(u8 *imageData, const u32 format_gl, i32 x, i32 y);
    
    void    bindAtlas2D(Texture &tex, u32 shader_id, u32 slot);
    void    bindAtlas2D(Texture &tex, shader::Shader shader, u32 slot);
    
    void    bind2D(Texture &tex, u32 slot);
    void    bind2D(TexGL texgl, const u32 slot);

    void    set_onload_flipv(bool enabled);
    void    free_tex2D(TexGL *texgl);


    /* TYPE DEFS */
    struct Texture
    {
        TextureID id;

        TextureRef operator()();
        const adobo::vec4f& operator[](i32 i);
        bool operator==(Texture &other);
        bool operator!=(Texture &other);

    };

    struct TextureRef
    {
        TexGL                 &id;
        adobo::vec2f          &tex_dim;
        const adobo::vec4f   *&sub_tex;
        i32                   &sub_n;

        TextureRef &operator=(const TextureRef &other);
    };

    template<i32 MAX_T>
    struct TextureSoa {
        ggb::Sparse<MAX_T>   sparse;
        TexGL                tex_ids[MAX_T]       = {};
        adobo::vec2f         tex_dim[MAX_T]       = {}; // dimensions x, y
        const adobo::vec4f  *sub_tex[MAX_T]       = {}; // contains tex_uv for tex and guaranteed to atleast 1 
        i32                  sub_n[MAX_T]         = {};
        i32                  max_capacity         = MAX_T;

        TextureRef operator[](i32 index);
        TextureRef operator()(TextureID tex_id);
        TextureRef operator()(Texture &tex);
    };


    /* FUNC DEF */
    
    /* Texture */
    inline TextureRef 
    Texture::operator()()
    {
        return g_textures(Texture::id);
    }

    inline const adobo::vec4f&
    Texture::operator[](i32 i)
    {
        return (*this)().sub_tex[i];
    }

    inline bool 
    Texture::operator==(Texture &other)
    {
        return id == other.id;
    }

    inline bool 
    Texture::operator!=(Texture &other)
    {
        return id != other.id;
    }

    
    /* TextureRef */
    inline TextureRef&
    TextureRef::operator=(const TextureRef &other)
    {
        id = other.id;
        tex_dim = other.tex_dim;
        sub_tex = other.sub_tex;
        sub_n = other.sub_n;

        return *this;
    }


    /* TextureSoa */
    template <i32 MAX_T>
    inline TextureRef 
    TextureSoa<MAX_T>::operator[](i32 index)
    {
        return TextureRef{
            .id = tex_ids[index],
            .tex_dim = tex_dim[index],
            .sub_tex = sub_tex[index],
            .sub_n = sub_n[index],
        };
    }

    template <i32 MAX_T>
    inline TextureRef 
    TextureSoa<MAX_T>::operator()(TextureID tex_id)
    {
        return (*this)[sparse[tex_id]];
    }

    template <i32 MAX_T>
    inline TextureRef 
    TextureSoa<MAX_T>::operator()(Texture &tex)
    {
        return (*this)(tex.id);
    }
}