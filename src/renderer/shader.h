#pragma once
#include "ggb/cglm.h"
#include "binassets/binasset_read_decl.h"
#include "types.h"

namespace shader 
{
    
    struct Shader 
    {
        const char *vertPath;
        const char *fragPath;
        u32 id = 0;
    };

    void create(Shader &shader, const binassets::AssetShader &vert, binassets::AssetShader &frag);
    void create(Shader &shader, const char *vertPath, const char *fragPath);
    
    void bind(Shader &shader);
    void bind(u32 shader_id);
    void unbind(void);

    void set_uniform_mat4(const Shader &shader, const char *name, const mat4 value);
    void set_uniform_vec4(const Shader &shader, const char *name, const vec4 value);
    void set_uniform_vec3(const Shader &shader, const char *name, const vec3 value);
    void set_uniform_vec2(const Shader &shader, const char *name, const vec2 value);
    void set_uniform_f32(const Shader &shader, const char *name, const f32 value);
    void set_uniform_i32(const Shader &shader, const char *name, const i32 value);
    
    // shader id
    void set_uniform_mat4(const u32 shader_id, const char *name, const mat4 value);
    void set_uniform_vec4(const u32 shader_id, const char *name, const vec4 value);
    void set_uniform_vec3(const u32 shader_id, const char *name, const vec3 value);
    void set_uniform_vec2(const u32 shader_id, const char *name, const vec2 value);
    void set_uniform_f32(const u32 shader_id, const char *name, const f32 value);
    void set_uniform_i32(const u32 shader_id, const char *name, const i32 value);

    
}