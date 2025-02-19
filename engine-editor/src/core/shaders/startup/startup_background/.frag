#version 330 core

out vec4 FragColor;

in vec2 v_uv;

uniform float time;

void main() {
    // Normalize UV coordinates to center the effect
    vec2 uv = v_uv * 2.0 - 1.0;

    // Calculate the distance from the center
    float dist = length(uv);

    // Create a ripple effect using sine waves and time
    float ripple = sin(10.0 * dist - time * 2.0);

    // Map ripple to a color gradient
    vec3 color = vec3(0.5 + 0.5 * cos(time + vec3(0.0, 2.0, 4.0)));

    // Modulate the color intensity by the ripple effect
    float intensity = smoothstep(0.0, 1.0, ripple);

    // Final color output
    FragColor = vec4(color * intensity, 1.0);
}