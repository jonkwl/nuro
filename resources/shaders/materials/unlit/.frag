#version 330 core

out vec4 FragColor;

in vec2 v_uv;

uniform vec4 baseColor;

uniform vec2 tiling;
uniform vec2 offset;

uniform bool enableTexture;
uniform sampler2D baseTexture;

void main()
{
    vec2 uv = v_uv * tiling + offset;
    vec4 color = baseColor;

    if (enableTexture) {
        color = texture(baseTexture, uv) * color;
    }

    FragColor = color;
}
