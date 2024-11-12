#version 330 core

uniform sampler2D inputTexture;
uniform vec2 resolution;

in vec2 uv;
layout (location = 0) out vec3 downsample;

void main()
{
    vec2 srcTexelSize = 1.0 / resolution;
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    vec3 a = texture(inputTexture, vec2(uv.x - 2*x, uv.y + 2*y)).rgb;
    vec3 b = texture(inputTexture, vec2(uv.x,       uv.y + 2*y)).rgb;
    vec3 c = texture(inputTexture, vec2(uv.x + 2*x, uv.y + 2*y)).rgb;

    vec3 d = texture(inputTexture, vec2(uv.x - 2*x, uv.y)).rgb;
    vec3 e = texture(inputTexture, vec2(uv.x,       uv.y)).rgb;
    vec3 f = texture(inputTexture, vec2(uv.x + 2*x, uv.y)).rgb;

    vec3 g = texture(inputTexture, vec2(uv.x - 2*x, uv.y - 2*y)).rgb;
    vec3 h = texture(inputTexture, vec2(uv.x,       uv.y - 2*y)).rgb;
    vec3 i = texture(inputTexture, vec2(uv.x + 2*x, uv.y - 2*y)).rgb;

    vec3 j = texture(inputTexture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 k = texture(inputTexture, vec2(uv.x + x, uv.y + y)).rgb;
    vec3 l = texture(inputTexture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 m = texture(inputTexture, vec2(uv.x + x, uv.y - y)).rgb;

    downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;
    downsample = max(downsample, 0.0001f);
}