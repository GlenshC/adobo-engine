#version 460 core

layout(location = 0) in vec2 aPos;

uniform mat4 u_projection; // your camera/projection matrix

void main()
{
    // z=0, w=1 for 2D
    gl_Position = u_projection * vec4(aPos, 0.0, 1.0);
}
