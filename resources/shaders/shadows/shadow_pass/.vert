#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normals_in;
layout(location = 2) in vec2 textureCoords_in;

uniform mat4 lightSpace;
uniform mat4 model;

void main()
{
    gl_Position = lightSpace * model * vec4(position_in, 1.0);
}
