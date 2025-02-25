#version 330 core

layout(location = 0) in vec3 position_in;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(position_in, 1.0);
}
