#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 uv_in;
layout(location = 3) in vec3 tangent_in;
layout(location = 4) in vec3 bitangent_in;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_fragmentPosition;
out vec4 v_fragmentLightSpacePosition;

void main()
{
    v_normal = normalMatrix * normal_in;
    v_uv = uv_in;
    v_tangent = normalMatrix * tangent_in;
    v_bitangent = normalMatrix * bitangent_in;
    v_fragmentPosition = vec3(modelMatrix * vec4(position_in, 1.0));
    v_fragmentLightSpacePosition = lightSpaceMatrix * vec4(v_fragmentPosition, 1.0);

    gl_Position = mvpMatrix * vec4(position_in, 1.0);
}
