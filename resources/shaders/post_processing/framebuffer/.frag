#version 330 core

out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D screenTexture;

uniform vec2 screenResolution;

uniform float contrast;

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

void main() 
{
    vec2 uv = textureCoords;
    vec4 color = texture(screenTexture, uv);
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

    // Contrast
    color.rgb = ((color.rgb - 0.5) * contrast) + 0.5;
    color.rgb = clamp(color.rgb, 0.0, 1.0);

    // Vignette
    if(vignette){
        vec2 scaledUV = vec2((uv.x - center.x) / vignetteRoundness, uv.y - center.y);
        float vignetteDist = length(scaledUV);
        float vignette = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, vignetteDist);
        color.rgba *= mix(vignetteColor, vec4(1.0), vignette);
    }
    
    FragColor = color;
}
