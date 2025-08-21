#pragma once
#include "types.h"
#include "ggb/sparse.h"
#include "renderer/shader.h"
#include "binassets/binasset_read_decl.h"

#define MAX_TEXTURES 12 
namespace texture
{

    /* TYPES */
    typedef i32 TextureID;
    typedef u32 TexGL;
    
    struct TextureRef;
    struct Texture;
    struct TextureManager;

    
    /* GLOBALS */
    extern TextureManager g_textures;


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

    struct TextureManager {
        char                *_bp;
        TexGL               *tex_ids;
        adobo::vec2f        *tex_dim;
        const adobo::vec4f **sub_tex; // contains tex_uv for tex and guaranteed to atleast 1 
        i32                 *sub_n;
        ggb::Sparse<i32>     sparse;
        
        size_t     size;
        size_t     capacity;

        void       init(size_t n);
        int        reserve(size_t new_cap);
        TextureRef operator[](size_t index);
        TextureRef operator()(TextureID tex_id);
        TextureRef operator()(Texture &tex);
    };

    inline void TextureManager::init(size_t n)
    {
        if (n > 0 && _bp)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*tex_ids) * n +
                sizeof(*tex_dim) * n +
                sizeof(*sub_tex) * n +
                sizeof(*sub_n)   * n
            );
            if (!mem)
            {
                DEBUG_ERR("Sparse: BAD ALLOCATION\n");
                return;
            }
            size_t offset = 0;
            _bp = mem;

            tex_ids = (TexGL *)               (mem);
            tex_dim = (adobo::vec2f  *)       (mem + (offset += sizeof(*tex_ids) * n));
            sub_tex = (const adobo::vec4f **) (mem + (offset += sizeof(*tex_dim) * n));
            sub_n =   (i32 *)                 (mem + (offset += sizeof(*sub_tex) * n));

            sparse.init(n);
            capacity = n;
        }
    }

    inline int TextureManager::reserve(size_t new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            _bp,
            sizeof(*tex_ids) * new_cap +
            sizeof(*tex_dim) * new_cap +
            sizeof(*sub_tex) * new_cap +
            sizeof(*sub_n)   * new_cap
        );
        if (!mem)
        {
            DEBUG_ERR("TextureManager: Resize Error\n");
            return 1;
        }
        size_t boffset = 
            sizeof(*tex_ids) * capacity +
            sizeof(*tex_dim) * capacity +
            sizeof(*sub_tex) * capacity;

        size_t offset = 0;
        _bp = mem;
        tex_ids = (TexGL *)               (mem); 
        tex_dim = (adobo::vec2f *)        (mem + (offset += sizeof(*tex_ids) * new_cap));
        sub_tex = (const adobo::vec4f **) (mem + (offset += sizeof(*tex_dim) * new_cap));
        sub_n   = (i32 *)                 (mem + (offset += sizeof(*sub_tex) * new_cap));

        if (new_cap >= (capacity << 1)) // memcpy
        {
            memcpy(sub_n  , (_bp), sizeof(*sub_n) * capacity);
            memcpy(sub_tex, (_bp + (boffset -= sizeof(*sub_tex) * capacity)), sizeof(*sub_tex) * capacity);
            memcpy(tex_dim, (_bp + (boffset -= sizeof(*tex_dim) * capacity)), sizeof(*tex_dim) * capacity);
        }
        else
        {
            memmove(sub_n  , (_bp), sizeof(*sub_n) * capacity);
            memmove(sub_tex, (_bp + (boffset -= sizeof(*sub_tex) * capacity)), sizeof(*sub_tex) * capacity);
            memmove(tex_dim, (_bp + (boffset -= sizeof(*tex_dim) * capacity)), sizeof(*tex_dim) * capacity);
        }
        capacity = new_cap;
        return 0; 
    }



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
    inline TextureRef 
    TextureManager::operator[](size_t index)
    {
        return TextureRef{
            .id = tex_ids[index],
            .tex_dim = tex_dim[index],
            .sub_tex = sub_tex[index],
            .sub_n = sub_n[index],
        };
    }

    inline TextureRef 
    TextureManager::operator()(TextureID tex_id)
    {
        return (*this)[sparse[tex_id]];
    }

    inline TextureRef 
    TextureManager::operator()(Texture &tex)
    {
        return (*this)(tex.id);
    }
}