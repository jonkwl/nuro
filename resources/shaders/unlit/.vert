#version 330 core
layout (location = 0) in vec3 local;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(local, 1.0);
}