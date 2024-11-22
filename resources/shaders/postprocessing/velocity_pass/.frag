#version 330 core

out vec2 FragColor;

smooth in vec4 v_position;
smooth in vec4 v_previousPosition;

void main() {
    vec2 a = (v_position.xy / v_position.w) * 0.5 + 0.5;
    vec2 b = (v_previousPosition.xy / v_previousPosition.w) * 0.5 + 0.5;
    FragColor = a - b;
}