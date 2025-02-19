#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 2) in vec2 uv_in;

uniform mat4 mvpMatrix;

out vec2 v_uv;

void main()
{
    v_uv = uv_in;

    gl_Position = mvpMatrix * vec4(position_in, 1.0);
}
