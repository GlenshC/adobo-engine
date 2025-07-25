#pragma once
#include "types.h"
#include "renderer/texture.h"
#include "renderer/shader.h"

namespace atlas
{
    struct Atlas{
        f32 tile_width = 0;
        f32 tile_height = 0;
        texture::Texture texture;
        i32 tile_count = 0;
    };
    

    void bind(shader::Shader &shader, Atlas &atlas, const u32 slot);
    void bind(const u32 shader_id, Atlas &atlas, const u32 slot);
    void create(Atlas &atlas, const char *image_path, int tile_width, int tile_height);
    void destroy(Atlas &atlas);
}
// Calculate UV min/max for a tile index (if needed outside shader)
// void atlas_get_uv(const Atlas* atlas, int index, float* out_uv_min, float* out_uv_max);