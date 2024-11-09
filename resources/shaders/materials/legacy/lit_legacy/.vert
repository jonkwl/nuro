#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normals_in;
layout(location = 2) in vec2 textureCoords_in;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 lightSpace;

out vec3 v_normals;
out vec3 v_fragmentPosition;
out vec4 v_fragmentLightSpacePosition;
out vec2 v_textureCoords;

void main()
{
    v_normals = mat3(transpose(inverse(model))) * normals_in;
    v_fragmentPosition = vec3(model * vec4(position_in, 1.0));
    v_fragmentLightSpacePosition = lightSpace * vec4(v_fragmentPosition, 1.0);
    v_textureCoords = textureCoords_in;

    gl_Position = mvp * vec4(position_in, 1.0);
}
