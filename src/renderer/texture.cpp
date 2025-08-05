
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <glad/gl.h>

#include "renderer/texture.h"
#include "renderer/shader.h"

#include "util/debug.h"

namespace texture
{
    void set_onload_flipv(bool enabled)
    {
        stbi_set_flip_vertically_on_load(enabled);
    }

    void load2D(Texture &tex, const char *path, const u32 format_gl)
    {
        i32 channels, x, y;

        u8 *imageData = stbi_load(
            path, &x, &y, &channels, 0
        );

        glGenTextures(1, &tex.id);
        glBindTexture(GL_TEXTURE_2D, tex.id);

        glTexParameteri(
            GL_TEXTURE_2D, 
            GL_TEXTURE_MIN_FILTER, GL_NEAREST
        ); // No mipmaps, nearest neighbor
        glTexParameteri(
            GL_TEXTURE_2D, 
            GL_TEXTURE_MAG_FILTER, GL_NEAREST
        );
        glTexParameteri(
            GL_TEXTURE_2D, 
            GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
        ); // Prevents texture bleeding at edges
        glTexParameteri(
            GL_TEXTURE_2D, 
            GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
        );

        if (imageData)
        {
            DEBUG_LOG("Texture %s Loaded.\n", path);
            glTexImage2D(
                GL_TEXTURE_2D, 0, format_gl, 
                x, y, 0, format_gl, 
                GL_UNSIGNED_BYTE, imageData
            );
            // glGenerateMipmap(GL_TEXTURE_2D);
            tex.width = x;
            tex.height = y;
        }
        stbi_image_free(imageData);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind2D(Texture &texture, const u32 slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture.id); // FIXME
    }

    void free(u32 *tex_id)
    {
        glDeleteTextures(1, tex_id);
        *tex_id = 0;
    }
}