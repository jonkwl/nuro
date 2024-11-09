#version 330 core

out vec4 FragColor;

in vec3 texture_coordinates;

uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, texture_coordinates);
}
