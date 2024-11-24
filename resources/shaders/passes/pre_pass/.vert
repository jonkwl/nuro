#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normal_in;

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;

out vec3 v_normal;

vec3 getNormal() {
    return normalize(normalMatrix * normal_in);
}

void main()
{
    v_normal = getNormal();
    gl_Position = mvpMatrix * vec4(position_in, 1.0);
}
