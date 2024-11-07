#version 330 core

out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D screenTexture;
uniform vec2 screenResolution;

uniform float exposure;
uniform float contrast;
uniform float gamma;

uniform bool chromaticAberration;
uniform float chromaticAberrationStrength;
uniform float chromaticAberrationRange;
uniform float chromaticAberrationRedOffset;
uniform float chromaticAberrationBlueOffset;

uniform bool vignette;
uniform float vignetteStrength;
uniform vec3 vignetteColor;
uniform float vignetteRadius;
uniform float vignetteSoftness;
uniform float vignetteRoundness;

uniform bool showShadowMap;

vec3 ACES(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() 
{
    vec2 uv = textureCoords;
    vec3 color = texture(screenTexture, uv).rgb;
    vec2 center = vec2(0.5, 0.5);
    vec2 toCenter = uv - center;
    float aspectRatio = screenResolution.x / screenResolution.y;

    // Chromatic Aberration
    if(chromaticAberration){
        float dist = length(toCenter);
        float aberration = smoothstep(chromaticAberrationRange, 1.0, dist) * chromaticAberrationStrength;
        vec2 redOffset = uv + toCenter * aberration * chromaticAberrationRedOffset;
        vec2 greenOffset = uv;
        vec2 blueOffset = uv - toCenter * aberration * chromaticAberrationBlueOffset;
        color.r = texture2D(screenTexture, redOffset).r;
        color.g = texture2D(screenTexture, greenOffset).g;
        color.b = texture2D(screenTexture, blueOffset).b;
    }

    // Vignette
    if(vignette){
        vec2 scaledUV = vec2((uv.x - center.x) / vignetteRoundness, uv.y - center.y);
        float vignetteDist = length(scaledUV);
        float vignetteFactor = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, vignetteDist);
        color *= mix(vignetteColor, vec3(1.0), vignetteFactor);
    }

    // Contrast
    color = ((color - 0.5) * contrast) + 0.5;
    color = clamp(color, 0.0, 1.0);

    // Exposure and Tone Mapping
    color = ACES(color * exposure);

    // Gamma
    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}
