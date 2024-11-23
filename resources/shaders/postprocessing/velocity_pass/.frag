#version 330 core

out vec2 FragColor;

smooth in vec4 v_position;
smooth in vec4 v_previousPosition;

uniform float intensity;

void main() {
    vec2 current = v_position.xy / v_position.w;
    vec2 previous = v_previousPosition.xy / v_previousPosition.w;
    vec2 color = (current - previous) * 0.5;
    color *= intensity;
    FragColor = color;
}