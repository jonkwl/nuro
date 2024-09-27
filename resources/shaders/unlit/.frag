#version 330 core
out vec4 FragColor;

// in vec4 base_color;
// in vec2 texture_coords;

uniform sampler2D base_texture;

void main()
{
    FragColor = vec4(0.8, 0.8, 0.8, 1.0);
    // FragColor = texture(base_texture, texture_coords) * base_color;
}