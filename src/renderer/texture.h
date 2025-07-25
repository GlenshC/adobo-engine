#pragma once
#include "types.h"

namespace texture
{
    struct Texture {
        f32 width = 0;
        f32 height = 0;
        u32 id = 0;
    };

    void load2D(Texture &tex, const char *path, const u32 format_gl);
    void bind2D(Texture &texture, u32 slot);
    void set_onload_flipv(bool enabled);
    void free(u32 *tex_id);
}