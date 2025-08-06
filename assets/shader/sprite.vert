#version 460 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

struct InstanceData {
    mat4    model;
    uint    uv_tex;
    // vec3    padding;
};

layout(std430, binding = 0) buffer SpriteInstances{
    InstanceData instances[];
};

uniform mat4 u_projection;
uniform vec4 u_atlas_info[16]; // per texture: (atlas_w, atlas_h, tile_w, tile_h)

out vec2 v_uv;
flat out float v_tex_index;

void main() {
    
    uint uv_tex = instances[gl_InstanceID].uv_tex;

    uint i_uv_index = (uv_tex & 0xffffu); 
    uint i_texture_id = ((uv_tex >> 16) & 0xffffu);

    vec4 info = u_atlas_info[i_texture_id];

    float atlas_w = info.x;
    float atlas_h = info.y;
    float tile_w  = info.z;
    float tile_h  = info.w;

    int columns = int(atlas_w / tile_w);
    int rows    = int(atlas_h / tile_h);

    // NEW LOGIC: Start from top-left, go down each row
    float col = mod(float(i_uv_index), float(columns));
    float row = floor(float(i_uv_index) / float(columns));
    // row = float(rows - 1) - row; // Flip Y (top to bottom)

    // Calculate UV region
    vec2 uv_min = vec2(col * tile_w, row * tile_h) / vec2(atlas_w, atlas_h);
    vec2 uv_max = uv_min + vec2(tile_w, tile_h) / vec2(atlas_w, atlas_h);

    v_uv = mix(uv_min, uv_max, vec2(a_uv.x, a_uv.y));
    v_tex_index = float(i_texture_id);

    mat4 model = instances[gl_InstanceID].model;
    vec4 world_pos = model * vec4(a_pos, 0.0, 1.0);
    gl_Position = u_projection * world_pos;


/*
    // for debug
    v_uv = a_uv;
    v_tex_index = 0.0;
    gl_Position = vec4(a_pos, 0.0, 1.0);
*/
}