#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normals_in;
layout(location = 2) in vec2 textureCoords_in;

uniform mat4 mvp;

out vec3 normals;
out vec2 textureCoords;

void main()
{
    normals = normals_in;
    textureCoords = textureCoords_in;

    gl_Position = mvp * vec4(position_in, 1.0);
}
