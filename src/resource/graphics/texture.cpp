
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <glad/gl.h>

#include "resource/graphics/texture.h"

#include "util/debug.h"

static const char *s_atlas_slots[] = {
    "u_atlas_info[0]",
    "u_atlas_info[1]",
    "u_atlas_info[2]",
    "u_atlas_info[3]",
    "u_atlas_info[4]",
    "u_atlas_info[5]",
    "u_atlas_info[6]",
    "u_atlas_info[7]",
    "u_atlas_info[8]",
    "u_atlas_info[9]",
    "u_atlas_info[10]",
    "u_atlas_info[11]",
    "u_atlas_info[12]",
    "u_atlas_info[13]",
    "u_atlas_info[14]",
    "u_atlas_info[15]",
};

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

    TextureSoa<MAX_TEXTURES> g_textures;

    void set_onload_flipv(bool enabled)
    {
        stbi_set_flip_vertically_on_load(enabled);
    }

    Texture loadAtlas2D(const char *path, const u32 formal_gl, f32 tile_w, f32 tile_h)
    {
        Texture tex_id = load2D(path, formal_gl, tile_w, tile_h);
        DEBUG_LOG("Atlas %s created %.1fx%.1f @ %.1fx%.1f\n", path, tile_w,tile_h, g_textures(tex_id).tex_dim.x, g_textures(tex_id).tex_dim.y);
        return tex_id;
    }

    Texture load2D(const char *path, const u32 format_gl, f32 tile_w, f32 tile_h)
    {
        u32 tex_gl;
        Texture id = texture::INVALID_TEX_ID;

        i32 channels, x, y;

        u8 *imageData = stbi_load(
            path, &x, &y, &channels, 0
        );

        glGenTextures(1, &tex_gl);
        glBindTexture(GL_TEXTURE_2D, tex_gl);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // No mipmaps, nearest neighbor
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents texture bleeding at edges
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (imageData)
        {
            DEBUG_LOG("Texture %s Loaded.\n", path);
            glTexImage2D(
                GL_TEXTURE_2D, 0, format_gl, 
                x, y, 0, format_gl, 
                GL_UNSIGNED_BYTE, imageData
            );
            // glGenerateMipmap(GL_TEXTURE_2D);
            id = g_textures.push(
                tex_gl, x, y, 
                (tile_w) ? (tile_w) : (x), 
                (tile_h) ? (tile_h) : (y)
            );
        }
        stbi_image_free(imageData);

        glBindTexture(GL_TEXTURE_2D, 0);
        
        return id;
    }
    
    Texture load2D(const char *path, const u32 format_gl)
    {
        return load2D(path, format_gl, 0, 0);
    }

    void bind2D(Texture tex_id, const u32 slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, g_textures(tex_id).id); // FIXME
    }
    
    void bindAtlas2D(u32 shader_id, Texture tex_id, u32 slot)
    {
        bind2D(tex_id, slot);
        shader::set_uniform_i32(
            shader_id, s_texture_slots[slot], 
            (i32) slot
        );
        shader::set_uniform_vec4(
            shader_id, s_atlas_slots[slot], 
            (float *) &g_textures(tex_id).tex_dim 
        );
    }
    void bindAtlas2D(shader::Shader shader, Texture tex_id, u32 slot)
    {
        bind2D(tex_id, slot);
        shader::set_uniform_i32(
            shader, s_texture_slots[slot], 
            (i32) slot
        );
        shader::set_uniform_vec4(
            shader, s_atlas_slots[slot], 
            (float *) &g_textures(tex_id).tex_dim 
        );
    }

    void free(Texture &tex_id)
    {
        u32 id = id;
        glDeleteTextures(1, &id);

        tex_id = INVALID_TEX_ID;
    }
}