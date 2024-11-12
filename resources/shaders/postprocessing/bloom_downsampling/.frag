#version 330 core

out vec3 FragColor;

uniform sampler2D inputTexture;
uniform vec2 inversedResolution;

in vec2 uv;

void main()
{
    float x = inversedResolution.x;
    float y = inversedResolution.y;

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

    FragColor = e*0.125;
    FragColor += (a+c+g+i)*0.03125;
    FragColor += (b+d+f+h)*0.0625;
    FragColor += (j+k+l+m)*0.125;
    FragColor = max(FragColor, 0.0001f);
}