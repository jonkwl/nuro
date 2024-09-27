#version 330 core
layout (location = 0) in vec3 local;
layout (location = 1) in vec4 i_color;
layout (location = 2) in vec2 i_tex_coord;

out vec4 base_color;
out vec2 texture_coords;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(local, 1.0);
    base_color = i_color;
    texture_coords = i_tex_coord;
}