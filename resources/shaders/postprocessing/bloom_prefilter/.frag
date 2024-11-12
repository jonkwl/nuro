#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D inputTexture;
uniform float threshold;
uniform float softThreshold;

vec3 applyThreshold(vec3 color) {
    float brightness = max(color.r, max(color.g, color.b));
    float knee = threshold * softThreshold;
    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0, knee * 2);
    soft = soft * soft / (knee * 4 + 0.00001);
    float contribution = max(0, brightness - threshold);
    contribution /= max(brightness, 0.00001);
    return color * contribution;
}

void main()
{
    //vec3 color = texture(inputTexture, uv).rgb;
    //FragColor = vec4(applyThreshold(color), 1.0);
    FragColor = vec4(vec3(softThreshold), 1.0);
}
