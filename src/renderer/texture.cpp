
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <glad/gl.h>
#include <cstdlib>

#include "renderer/texture.h"
#include "binassets/binasset_read.h"
#include "util/debug.h"
#include "core/constants.h"
// #include "nlohmann/tinyxml2.h"

static const char *s_texture_slots[] = {
    "u_textures[0]",
    "u_textures[1]",
    "u_textures[2]",
    "u_textures[3]",
    "u_textures[4]",
    "u_textures[5]",
    "u_textures[6]",
    "u_textures[7]",
    "u_textures[8]",
    "u_textures[9]",
    "u_textures[10]",
    "u_textures[11]",
    "u_textures[12]",
    "u_textures[13]",
    "u_textures[14]",
    "u_textures[15]",
};

namespace texture
{
    TextureManager g_textures;

    void init_TextureManager()
    {
        g_textures.init(ADOBO_ENGINE_MIN_TEXTURE);
    }

    static adobo::vec4f *calculate_uvs(f32 x, f32 y, f32 tile_w, f32 tile_h, i32 &n_out)
    {

        f32 cols = x / tile_w; // how many tiles per row
        f32 rows = y / tile_h; // how many tiles per column
        if (cols < 1)
            cols = 1;
        if (rows < 1)
            rows = 1;

        i32 size = (i32)(cols * rows);
        adobo::vec4f *arr = (adobo::vec4f *)std::malloc(size * sizeof(adobo::vec4f));
        if (arr == nullptr)
        {
            DEBUG_ERR("Cant malloc this fat ass SubTextureDims.\n");
            n_out = 0;
            return nullptr;
        }
        n_out = (i32)(rows*cols);

        f32 tile_u = tile_w / x; // fraction of texture per tile (width in UV space)
        f32 tile_v = tile_h / y; // fraction of texture per tile (height in UV space)

        for (i32 row = 0; row < (i32)rows; row++)
        {
            for (i32 col = 0; col < (i32)cols; col++)
            {
                i32 index = row * (i32)cols + col;

                f32 uv_min_x = col * tile_u;
                f32 uv_min_y = row * tile_v;
                f32 uv_max_x = uv_min_x + tile_u;
                f32 uv_max_y = uv_min_y + tile_v;

                arr[index] = {uv_min_x, uv_min_y, uv_max_x, uv_max_y};
            }
        }
        return arr;
    }
    static adobo::vec4f *calculate_uvs(binassets::SubTextureDims *dims, f32 x, f32 y, int sub_n)
    {

        adobo::vec4f *arr = (adobo::vec4f *)std::malloc(sub_n * sizeof(adobo::vec4f));
        if (arr == nullptr)
        {
            DEBUG_ERR("Cant malloc this fat ass SubTextureDims.\n");
            return nullptr;
        }

        for (int i = 0; i < sub_n; i++)
        {
            arr[i] = {
                dims[i].x / x, 
                dims[i].y / y,
                (dims[i].x + dims[i].width) / x, 
                (dims[i].y + dims[i].height)/ y
            };
        }
        return arr;
    }


    Texture create_tex2D(void)
    {
        return {g_textures.sparse.create_id()}; // this increments size
    }

    TextureRef create_tex2D(Texture &tex_out)
    {
        if (g_textures.size + ADOBO_ENGINE_REALLOC_MARGIN >= g_textures.capacity) 
        {
            g_textures.reserve(g_textures.capacity << 1);
        }
        g_textures.size++;
        tex_out.id = g_textures.sparse.create_id();
        return g_textures(tex_out.id);
    }

    void remove_tex2D(Texture &tex)
    {
        i32 index         = g_textures.sparse.remove(tex.id);
        i32 last_index    = g_textures.sparse.size;
        free_tex2D(&g_textures[index].id);
        g_textures[index] = g_textures[last_index];
        tex.id = g_textures.sparse.invalid_id();
    }

    Texture 
    loadAtlas2D(binassets::AssetAtlas &atlas, const u32 format_gl)
    {
        Texture tex = {g_textures.sparse.invalid_id()};
        TexGL tex_gl = load2D(atlas.data, format_gl, atlas.x, atlas.y);

        TextureRef data = create_tex2D(tex);
        data.id = tex_gl;
        data.tex_dim = {(float)atlas.x, (float)atlas.y};
        data.sub_tex = calculate_uvs(atlas.dims, atlas.x, atlas.y, atlas.subtex_n);
        data.sub_n = atlas.subtex_n;
        atlas.tex = tex;
        if (binassets::g_asset_free_on_load)
        {
            DEBUG_LOG("loadAtlas2D: Free on load.\n");
            std::free(atlas.data);
        }
        
        return tex;
    }

    Texture 
    loadAtlas2D(binassets::AssetIMG &img, const u32 format_gl, f32 tile_w, f32 tile_h)
    {
        Texture tex = {g_textures.sparse.invalid_id()};
        TexGL tex_gl = load2D(img.data, format_gl, img.x, img.y);

        i32 sub_n = 0;
        TextureRef data = create_tex2D(tex);
        data.id = tex_gl;
        data.tex_dim = {(float)img.x, (float)img.y};
        data.sub_tex = calculate_uvs(img.x, img.y, tile_w, tile_h, sub_n);
        data.sub_n = sub_n;

        return tex;
    }

    Texture 
    loadAtlas2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h)
    {
        i32 channels, x, y;

        u8 *imageData = stbi_load(
            path, &x, &y, &channels, 0
        );

        Texture tex = {g_textures.sparse.invalid_id()};
        TexGL tex_gl = load2D(imageData, format_gl, x, y);

        i32 sub_n = 0;
        TextureRef data = create_tex2D(tex);
        data.id = tex_gl;
        data.tex_dim = {(float)x, (float)y};
        data.sub_tex = calculate_uvs(x, y, tile_w, tile_h, sub_n);
        data.sub_n = sub_n;

        return tex;
    }
    
    TexGL 
    load2D(u8 *imageData, const u32 format_gl, i32 x, i32 y)
    {
        DEBUG_LOG("Loading Texture: %dx%d...\n", x, y);
        TexGL tex_gl;
        
        glGenTextures(1, &tex_gl);
        glBindTexture(GL_TEXTURE_2D, tex_gl);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // No mipmaps, nearest neighbor
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents texture bleeding at edges
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (imageData)
        {
            // glGetError(); // clear
            glTexImage2D(
                GL_TEXTURE_2D, 0, format_gl, 
                x, y, 0, format_gl, 
                GL_UNSIGNED_BYTE, imageData
            );
            glGenerateMipmap(GL_TEXTURE_2D);
            // GLenum err = glGetError();
            // DEBUG_LOG("GL error: %d\n", err);
            DEBUG_LOG("Texture Loaded : %dx%d\n", x, y);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        
        return tex_gl;
    }
    
    Texture load2D(const char *path, const u32 format_gl)
    {
        i32 channels, x, y;

        u8 *imageData = stbi_load(
            path, &x, &y, &channels, 0
        );

        Texture tex = {g_textures.sparse.invalid_id()};
        TexGL tex_gl = load2D(imageData, format_gl, x, y);

        i32 sub_n = 0;
        TextureRef data = create_tex2D(tex);
        data.id = tex_gl;
        data.tex_dim = {(float)x, (float)y};
        data.sub_tex = calculate_uvs(x, y, x, y, sub_n);
        data.sub_n = sub_n;
        
        stbi_image_free(imageData);

        return tex;
    }

    void bind2D(Texture &tex, const u32 slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, g_textures(tex).id); // FIXME
    }
    void bind2D(TexGL texgl, const u32 slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texgl);
    }
    
    void bindAtlas2D(Texture &tex, u32 shader_id, u32 slot)
    {
        bind2D(tex, slot);
        shader::set_uniform_i32(
            shader_id, s_texture_slots[slot], 
            (i32) slot
        );
    }

    void bindAtlas2D(Texture &tex, shader::Shader shader, u32 slot)
    {
        bindAtlas2D(tex, shader.id, slot);
    }

    void free_tex2D(TexGL *texgl)
    {
        glDeleteTextures(1, texgl);
    }

    void set_onload_flipv(bool enabled)
    {
        stbi_set_flip_vertically_on_load(enabled);
    }
}