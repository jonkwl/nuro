#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D inputTexture;
uniform vec2 resolution;

const float offset = 1.0 / 300.0;

void main()
{
    // FragColor = texture(inputTexture, uv);
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
