#version 330 core

out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D screenTexture;

uniform vec2 screenResolution;

uniform float contrast;

uniform float vignetteAmount;
uniform float vignetteFalloff;

void main() 
{
    vec4 color = texture(screenTexture, textureCoords);
    vec2 uv = vec2(textureCoords);

    /* // Contrast
    color.rgb = ((color.rgb - 0.5) * contrast) + 0.5;
    color.rgb = clamp(color.rgb, 0.0, 1.0); */

    // Vignette
    /* vec2 uv = 1.0 - textureCoords.xy;
    uv *=  1.0 - uv.yx;
    float vig = uv.x*uv.y * 15.0;
    vig = pow(vig, 0.25);
    FragColor = vec4(vig); */

    float dist = distance(uv, vec2(0.5, 0.5));
    color.rgb *= smoothstep(0.8, vignetteFalloff * 0.8, dist * (vignetteAmount + vignetteFalloff));

    FragColor = color;
}
