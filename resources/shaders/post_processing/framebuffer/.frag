#version 330 core

out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D screenTexture;

uniform vec2 screenResolution;

uniform float contrast;

uniform vec3 vignetteColor;
uniform float vignetteRadius;
uniform float vignetteSoftness;
uniform float vignetteRoundness;

void main() 
{
    vec2 uv = textureCoords;
    vec4 color = texture(screenTexture, uv);
    vec2 center = vec2(0.5, 0.5);

    // Contrast
    color.rgb = ((color.rgb - 0.5) * contrast) + 0.5;
    color.rgb = clamp(color.rgb, 0.0, 1.0);

    // Vignette
    float aspectRatio = screenResolution.x / screenResolution.y;
    vec2 scaledUV = vec2((uv.x - center.x) / vignetteRoundness, uv.y - center.y);
    float dist = length(scaledUV);
    float vignette = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, dist);
    color.rgb *= mix(vignetteColor, vec3(1.0), vignette);

    FragColor = color;
}
