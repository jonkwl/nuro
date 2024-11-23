#version 330 core

layout(location = 0) in vec2 position_in;
layout(location = 1) in vec2 uv_in;

out vec2 uv;

void main()
{
    uv = uv_in;

    gl_Position = vec4(position_in, 0.0, 1.0);
}