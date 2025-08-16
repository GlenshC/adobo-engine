#version 460 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

struct InstanceData {
    mat4    model;
    vec4    tex_uv; // normalized
    uint    tex_id; // index for sampler2d and also for u_atlas_info
};

layout(std430, binding = 0) buffer SpriteInstances{
    InstanceData instances[];
};

uniform mat4 u_projection;

out vec2 v_uv;
flat out int v_tex_index;

void main() {
    
    v_uv = mix(
        instances[gl_InstanceID].tex_uv.xy, 
        instances[gl_InstanceID].tex_uv.zw, 
        vec2(a_uv.x, a_uv.y)
    );

    v_tex_index = int(instances[gl_InstanceID].tex_id);

    gl_Position = u_projection * instances[gl_InstanceID].model * vec4(a_pos, 0.0, 1.0);

}