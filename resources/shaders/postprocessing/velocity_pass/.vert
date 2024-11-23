#version 330 core

layout(location = 0) in vec3 position_in;

uniform mat4 modelMatrix;
uniform mat4 previousModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 v_viewPosition;
out vec4 v_position;
out vec4 v_previousPosition;

void main()
{
    vec4 worldPosition = modelMatrix * vec4(position_in, 1.0);
    vec4 previousWorldPosition = previousModelMatrix * vec4(position_in, 1.0);

    v_viewPosition = viewMatrix * worldPosition;
    v_position = projectionMatrix * v_viewPosition;
    v_previousPosition = projectionMatrix * viewMatrix * previousWorldPosition;

    gl_Position = v_position;
}