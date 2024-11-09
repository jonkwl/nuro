#version 330 core

layout(location = 0) in vec3 position_in;

out vec3 texture_coordinates;

uniform mat4 view;
uniform mat4 projection;

void main() {
    texture_coordinates = position_in;
    vec4 position = projection * view * vec4(position_in, 1.0);
    gl_Position = position.xyww;
}
