#version 460 core

layout(location = 0) in vec2 a_pos;

uniform mat4 u_projection;
uniform mat4 u_model;

void main() {
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
