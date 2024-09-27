#version 330 core
out vec4 FragColor;

in vec4 base_color;
in vec2 texture_coords;

uniform sampler2D base_texture;

void main()
{
    FragColor = texture(base_texture, texture_coords) * base_color;

}