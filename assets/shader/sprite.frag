#version 460 core

in vec2 v_uv;
flat in int v_tex_index;

out vec4 FragColor;

// Bind up to 16 2D textures (indexed via v_tex_index)
uniform sampler2D u_textures[16];

void main() {
    int index = v_tex_index;

    if (index < 0 || index >= 16) {
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); // magenta = error
    } else {
        FragColor = texture(u_textures[index], v_uv);
        // if (texel.a < 0.5)
        //     discard;
    }
}