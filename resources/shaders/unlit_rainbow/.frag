#version 330 core

// Uniform variable for time
uniform float iTime;

// Output color of the fragment
out vec4 FragColor;

void main() {
    // Get normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    
    // Create a rainbow effect using sine waves
    // The x coordinate is used to calculate the color, with a time offset for animation
    float r = 0.5 + 0.5 * sin(6.2831 * (uv.x + iTime * 0.2 + 0.0));
    float g = 0.5 + 0.5 * sin(6.2831 * (uv.x + iTime * 0.2 + 0.33));
    float b = 0.5 + 0.5 * sin(6.2831 * (uv.x + iTime * 0.2 + 0.67));
    
    // Combine the colors into a vec3
    vec3 color = vec3(r, g, b);
    
    // Output the final color with full opacity
    FragColor = vec4(color, 1.0);
}