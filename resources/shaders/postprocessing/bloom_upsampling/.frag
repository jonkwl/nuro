#version 330 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!
uniform sampler2D inputTexture;
uniform float filterRadius;

in vec2 uv;
layout (location = 0) out vec3 upsample;

void main()
{
    float x = filterRadius;
    float y = filterRadius;

    vec3 a = texture(inputTexture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 b = texture(inputTexture, vec2(uv.x,     uv.y + y)).rgb;
    vec3 c = texture(inputTexture, vec2(uv.x + x, uv.y + y)).rgb;

    vec3 d = texture(inputTexture, vec2(uv.x - x, uv.y)).rgb;
    vec3 e = texture(inputTexture, vec2(uv.x,     uv.y)).rgb;
    vec3 f = texture(inputTexture, vec2(uv.x + x, uv.y)).rgb;

    vec3 g = texture(inputTexture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 h = texture(inputTexture, vec2(uv.x,     uv.y - y)).rgb;
    vec3 i = texture(inputTexture, vec2(uv.x + x, uv.y - y)).rgb;

    upsample = e*4.0;
    upsample += (b+d+f+h)*2.0;
    upsample += (a+c+g+i);
    upsample *= 1.0 / 16.0;
}