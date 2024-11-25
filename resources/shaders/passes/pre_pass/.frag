#version 330 core

out vec4 FragColor;

in vec3 v_viewNormal;

vec3 encodeNormalOutput(vec3 normal) {
    // remap from [-1, 1] to [0, 1]
    return normal * 0.5 + 0.5;
}

void main()
{
    // encode view space normal as color and set as output
    FragColor = vec4(encodeNormalOutput(v_viewNormal), 1.0);
}