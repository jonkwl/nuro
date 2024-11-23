#version 330 core

layout(location = 0) in vec3 position_in;

uniform mat4 modelMatrix;
uniform mat4 previousModelMatrix;
uniform mat4 viewProjectionMatrix;

smooth out vec4 v_position;
smooth out vec4 v_previousPosition;

void main()
{
    vec4 worldPosition = modelMatrix * vec4(position_in, 1.0);
    vec4 previousWorldPosition = previousModelMatrix * vec4(position_in, 1.0);

    v_position = viewProjectionMatrix * worldPosition;
    v_previousPosition = viewProjectionMatrix * previousWorldPosition;

    gl_Position = v_position;
}