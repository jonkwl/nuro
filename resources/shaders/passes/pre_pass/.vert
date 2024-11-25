#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normal_in;

uniform mat4 mvpMatrix;
uniform mat3 viewNormalMatrix;

out vec3 v_viewNormal;

vec3 getViewNormal() {
    return normalize(viewNormalMatrix * normal_in);
}

void main()
{
    v_viewNormal = getViewNormal();
    gl_Position = mvpMatrix * vec4(position_in, 1.0);
}