#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 2) in vec2 uv_in;

out vec2 v_uv;

void main() 
{
    v_uv = uv_in;

    gl_Position = vec4(vec2(position_in), 0.0, 1.0);
}