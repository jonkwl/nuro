#version 330 core

layout(location = 0) in vec3 position_in;

uniform mat4 mvpMatrix;
uniform mat4 previousMvpMatrix;

smooth out vec4 v_position;
smooth out vec4 v_previousPosition;

void main()
{
    v_position = mvpMatrix * vec4(position_in, 1.0);
    v_previousPosition = previousMvpMatrix * vec4(position_in, 1.0);

    gl_Position = v_position;
}