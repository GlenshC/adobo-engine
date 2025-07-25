#include "resource/atlas.h"
#include <ggb/cglm.h>
#include <glad/gl.h>

#include "util/logger.h"

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

namespace atlas
{
    // make sure to bind shader first
    void bind(shader::Shader &shader, Atlas &atlas, const u32 slot)
    {
        texture::bind2D(atlas.texture, slot);
        shader::set_uniform_i32(
            shader, s_texture_slots[slot], 
            (i32) slot
        );
        shader::set_uniform_vec4(
            shader, s_atlas_slots[slot], 
            (float *) &atlas 
        );
    }

    void bind(const u32 shader_id, Atlas &atlas, const u32 slot)
    {
        texture::bind2D(atlas.texture, slot);
        shader::set_uniform_i32(
            shader_id, s_texture_slots[slot], 
            slot
        );
        shader::set_uniform_vec4(
            shader_id, s_atlas_slots[slot], 
            (float *) &atlas 
        );
        // DEBUG_LOG("%s %s\n", s_atlas_slots[slot], s_texture_slots[slot]);
    }

    void create(Atlas &atlas, const char *image_path, int tile_width, int tile_height)
    {
        texture::load2D(atlas.texture, image_path, GL_RGBA); 
        int atlas_width = 
            atlas.texture.width, 
            atlas_height = atlas.texture.height;

        atlas.tile_count = (atlas_width / tile_width) * (atlas_height / tile_height);

        atlas.tile_width = (float)tile_width;
        atlas.tile_height = (float)tile_height;
        DEBUG_LOG("Atlas %s created %dx%d @ %dx%d\n", image_path, tile_width,tile_height,atlas_width,atlas_height);
    }

    void destroy(Atlas &atlas)
    {
        texture::free(&atlas.texture.id);
        atlas.tile_count = 0;
    }
}