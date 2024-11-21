#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D hdrBuffer;
uniform sampler2D depthBuffer;
uniform sampler2D bloomBuffer;

uniform vec2 resolution;

struct Configuration {
    float exposure;
    float contrast;
    float gamma;

    bool bloom;
    float bloomIntensity;
    vec3 bloomColor;
    bool lensDirt;
    sampler2D lensDirtTexture;
    float lensDirtIntensity;

    bool chromaticAberration;
    float chromaticAberrationIntensity;
    float chromaticAberrationRange;
    float chromaticAberrationRedOffset;
    float chromaticAberrationBlueOffset;

    bool vignette;
    float vignetteIntensity;
    vec3 vignetteColor;
    float vignetteRadius;
    float vignetteSoftness;
    float vignetteRoundness;
};
uniform Configuration configuration;

float near = 0.3;
float far = 100.0;

vec3 ACES(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec4 DepthColor() {
    float depth = texture(depthBuffer, uv).r;
    float linearDepth = LinearizeDepth(depth);
    float normalizedDepth = (linearDepth - near) / (far - near);
    return vec4(vec3(normalizedDepth), 1.0);
}

vec3 chromaticAberration(vec3 color) {
    vec2 center = vec2(0.5, 0.5);
    vec2 toCenter = uv - center;
    float dist = length(toCenter);
    float aberration = smoothstep(configuration.chromaticAberrationRange, 1.0, dist) * configuration.chromaticAberrationIntensity;
    vec2 redOffset = uv + toCenter * aberration * configuration.chromaticAberrationRedOffset;
    vec2 greenOffset = uv;
    vec2 blueOffset = uv - toCenter * aberration * configuration.chromaticAberrationBlueOffset;
    color.r = texture2D(hdrBuffer, redOffset).r;
    color.g = texture2D(hdrBuffer, greenOffset).g;
    color.b = texture2D(hdrBuffer, blueOffset).b;
    return color;
}

vec3 bloom(vec3 color) {
    vec3 bloomSample = texture(bloomBuffer, uv).rgb * configuration.bloomIntensity * configuration.bloomColor;
    vec3 lensDirtSample = vec3(0.0);
    if (configuration.lensDirt) {
        lensDirtSample = texture(configuration.lensDirtTexture, vec2(uv.x, 1.0 - uv.y)).rgb * configuration.lensDirtIntensity;
    }
    color = mix(color, color + bloomSample + bloomSample * lensDirtSample, vec3(1.0));
    return color;
}

vec3 vignette(vec3 color) {
    vec2 center = vec2(0.5, 0.5);
    vec2 scaledUV = vec2((uv.x - center.x) / configuration.vignetteRoundness, uv.y - center.y);
    float vignetteDist = length(scaledUV);
    float vignetteFactor = smoothstep(configuration.vignetteRadius, configuration.vignetteRadius - configuration.vignetteSoftness, vignetteDist);
    color *= mix(configuration.vignetteColor, vec3(1.0), vignetteFactor);
    return color;
}

void main()
{
    // FragColor = DepthColor(); return;

    float aspectRatio = resolution.x / resolution.y;

    vec3 color = texture(hdrBuffer, uv).rgb;

    // Chromatic Aberration
    if (configuration.chromaticAberration) {
        color = chromaticAberration(color);
    }

    // Bloom
    if (configuration.bloom) {
        color = bloom(color);
    }

    // Vignette
    if (configuration.vignette) {
        color = vignette(color);
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
