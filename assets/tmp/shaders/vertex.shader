#version 330 core
layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec4 i_col;

out vec4 base_color;

void main()
{
    gl_Position = vec4(i_pos, 1.0);
    base_color = i_col;
}