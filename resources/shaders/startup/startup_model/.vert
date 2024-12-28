#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 uv_in;
layout(location = 3) in vec3 tangent_in;
layout(location = 4) in vec3 bitangent_in;

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;

out vec3 v_normal;
out vec2 v_uv;

void main(){
    v_normal = normalize(normalMatrix * normal_in);
    v_uv = uv_in;
    
    gl_Position = mvpMatrix * vec4(position_in, 1.0);
}