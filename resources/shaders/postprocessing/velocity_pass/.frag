#version 330 core

out vec2 FragColor;

smooth in vec4 v_position;
smooth in vec4 v_previousPosition;

void main() {
    vec2 current = v_position.xy / v_position.w;
    vec2 previous = v_previousPosition.xy / v_previousPosition.w;
    FragColor = (current - previous) * 0.5;
}
