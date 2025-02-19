#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D ssaoInput;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float blurResult = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            blurResult += texture(ssaoInput, uv + offset).r;
        }
    }
    float occlusion = blurResult / (4.0 * 4.0);

    FragColor = vec4(occlusion, 0.0, 0.0, 1.0);
}
