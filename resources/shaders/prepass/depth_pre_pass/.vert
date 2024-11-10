#version 330 core

layout(location = 0) in vec3 position_in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main()
{
    gl_Position = viewMatrix * modelMatrix * vec4(position_in, 1.0);
}
