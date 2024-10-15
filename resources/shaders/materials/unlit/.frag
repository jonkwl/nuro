#version 330 core

out vec4 FragColor;

in vec3 normals;
in vec2 textureCoords;

uniform vec4 baseColor;
uniform bool enableTexture;
uniform sampler2D baseTexture;

void main()
{
    vec4 color = baseColor;

    if(enableTexture){
        color = texture(baseTexture, textureCoords) * color;
    }

    FragColor = color;
}