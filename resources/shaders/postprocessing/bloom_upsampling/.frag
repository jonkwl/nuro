#version 330 core

out vec3 FragColor;

uniform sampler2D inputTexture;
uniform float filterRadius;
uniform float aspectRatio;

in vec2 uv;

void main()
{
    float x = filterRadius;
    float y = filterRadius * aspectRatio;

    vec3 a = texture(inputTexture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 b = texture(inputTexture, vec2(uv.x,     uv.y + y)).rgb;
    vec3 c = texture(inputTexture, vec2(uv.x + x, uv.y + y)).rgb;

    vec3 d = texture(inputTexture, vec2(uv.x - x, uv.y)).rgb;
    vec3 e = texture(inputTexture, vec2(uv.x,     uv.y)).rgb;
    vec3 f = texture(inputTexture, vec2(uv.x + x, uv.y)).rgb;

    vec3 g = texture(inputTexture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 h = texture(inputTexture, vec2(uv.x,     uv.y - y)).rgb;
    vec3 i = texture(inputTexture, vec2(uv.x + x, uv.y - y)).rgb;

    FragColor = e*4.0;
    FragColor += (b+d+f+h)*2.0;
    FragColor += (a+c+g+i);
    FragColor *= 1.0 / 16.0;
}