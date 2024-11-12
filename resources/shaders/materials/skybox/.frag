#version 330 core

out vec4 FragColor;

in vec3 texture_coordinates;

uniform samplerCube skybox;
uniform float emission;

void main() {
    FragColor = texture(skybox, texture_coordinates) * emission;
}
