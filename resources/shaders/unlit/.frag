#version 330 core

out vec4 FragColor;         // Output color of the fragment

in vec3 normals;            // Input normal vector
in vec2 textureCoords;      // Input texture coordinates

uniform vec4 baseColor; // Base color
uniform sampler2D baseTexture; // Texture sampler

void main()
{
    FragColor = baseColor;
    // FragColor = texture(baseTexture, textureCoords) * baseColor;
}