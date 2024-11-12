#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;

uniform vec2 resolution;

struct Configuration {
    float exposure;
    float contrast;
    float gamma;

    float bloomIntensity;
    vec3 bloomColor;
    float bloomBlend;

    bool lensDirt;
    sampler2D lensDirtTexture;
    float lensDirtIntensity;

    bool chromaticAberration;
    float chromaticAberrationStrength;
    float chromaticAberrationRange;
    float chromaticAberrationRedOffset;
    float chromaticAberrationBlueOffset;

    bool vignette;
    float vignetteStrength;
    vec3 vignetteColor;
    float vignetteRadius;
    float vignetteSoftness;
    float vignetteRoundness;
};
uniform Configuration configuration;

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
    vec2 center = vec2(0.5, 0.5);
    vec2 toCenter = uv - center;
    float aspectRatio = resolution.x / resolution.y;

    vec3 color = texture(hdrBuffer, uv).rgb;

    // Bloom
    vec3 bloom = texture(bloomBuffer, uv).rgb * configuration.bloomIntensity * configuration.bloomColor;
    vec3 lensDirt = vec3(0.0);
    if(configuration.lensDirt){
        lensDirt = texture(configuration.lensDirtTexture, vec2(uv.x, 1.0 - uv.y)).rgb * configuration.lensDirtIntensity;
    }
    color = mix(color, bloom + bloom * lensDirt, vec3(configuration.bloomBlend));
 
    // Not compatible with bloom yet
    /* // Chromatic Aberration
    if (configuration.chromaticAberration) {
        float dist = length(toCenter);
        float aberration = smoothstep(configuration.chromaticAberrationRange, 1.0, dist) * configuration.chromaticAberrationStrength;
        vec2 redOffset = uv + toCenter * aberration * configuration.chromaticAberrationRedOffset;
        vec2 greenOffset = uv;
        vec2 blueOffset = uv - toCenter * aberration * configuration.chromaticAberrationBlueOffset;
        color.r = texture2D(hdrBuffer, redOffset).r;
        color.g = texture2D(hdrBuffer, greenOffset).g;
        color.b = texture2D(hdrBuffer, blueOffset).b;
    } */

    // Vignette
    if (configuration.vignette) {
        vec2 scaledUV = vec2((uv.x - center.x) / configuration.vignetteRoundness, uv.y - center.y);
        float vignetteDist = length(scaledUV);
        float vignetteFactor = smoothstep(configuration.vignetteRadius, configuration.vignetteRadius - configuration.vignetteSoftness, vignetteDist);
        color *= mix(configuration.vignetteColor, vec3(1.0), vignetteFactor);
    }

    // Contrast
    color = ((color - 0.5) * configuration.contrast) + 0.5;
    color = clamp(color, 0.0, 1.0);

    // Exposure and Tone Mapping
    color = ACES(color * configuration.exposure);

    // Gamma
    color = pow(color, vec3(1.0 / configuration.gamma));

    FragColor = vec4(color, 1.0);
}
