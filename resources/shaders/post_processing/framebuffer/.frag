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
uniform vec4 vignetteColor;
uniform float vignetteRadius;
uniform float vignetteSoftness;
uniform float vignetteRoundness;

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
        float vignette = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, vignetteDist);
        color.rgb *= mix(vignetteColor.rgb, vec3(1.0), vignette);
    }

    // Contrast
    color.rgb = ((color.rgb - 0.5) * contrast) + 0.5;
    color.rgb = clamp(color.rgb, 0.0, 1.0);

    // Exposure / Tone Mapping
    vec3 hdrColor = color;
    vec3 toneMapped = ACES(hdrColor * exposure);

    // Gamma
    vec3 gammaCorrected = pow(toneMapped, vec3(1.0 / gamma));

    FragColor = vec4(gammaCorrected, 1.0);
}
