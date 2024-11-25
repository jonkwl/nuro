#version 330 core

out vec4 FragColor;

in vec4 v_viewPosition;
in vec4 v_position;
in vec4 v_previousPosition;

uniform float intensity;

void main() {
    // calculate velocity
    vec2 current = v_position.xy / v_position.w;
    vec2 previous = v_previousPosition.xy / v_previousPosition.w;
    vec2 velocity = (current - previous) * 0.5;
    velocity *= intensity;

    // get depth in view space
    float viewSpaceDepth = v_viewPosition.z;
    
    // RED CHANNEL = x velocity | GREEN CHANNEL = y velocity | BLUE CHANNEL = view space depth
    FragColor = vec4(velocity.x, velocity.y, viewSpaceDepth, 1.0);
}