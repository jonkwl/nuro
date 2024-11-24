#version 330 core 

out vec4 FragColor;

in vec2 uv;

uniform sampler2D hdrInput;
uniform sampler2D aoInput;

void main() {
    vec4 hdrSample = texture(hdrInput, uv);
    float occlusion = texture(aoInput, uv).r;

    FragColor = hdrSample * occlusion;
}