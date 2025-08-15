#pragma once
#include "ggb/cglm.h"
#include "imgbin.h"
#include "types.h"

namespace shader 
{
    
    struct Shader 
    {
        const char *vertPath;
        const char *fragPath;
        u32 id = 0;
    };

    void create_fsource(Shader &shader, const char *vertSource, const char *fragSource);
    void create(Shader &shader, const ggb::AssetShader &vert, ggb::AssetShader &frag);
    void create(Shader &shader, const char *vertPath, const char *fragPath);
    
    void bind(Shader &shader);
    void bind(u32 shader_id);
    void unbind(void);

    void set_uniform_mat4(const Shader &shader, const char *name, mat4 value);
    void set_uniform_vec4(const Shader &shader, const char *name, vec4 value);
    void set_uniform_vec2(const Shader &shader, const char *name, vec2 value);
    void set_uniform_f32(const Shader &shader, const char *name, f32 value);
    void set_uniform_i32(const Shader &shader, const char *name, i32 value);

    // shader id
    void set_uniform_mat4(const u32 shader_id, const char *name, mat4 value);
    void set_uniform_vec4(const u32 shader_id, const char *name, vec4 value);
    void set_uniform_vec2(const u32 shader_id, const char *name, vec2 value);
    void set_uniform_f32(const u32 shader_id, const char *name, f32 value);
    void set_uniform_i32(const u32 shader_id, const char *name, i32 value);

    
}