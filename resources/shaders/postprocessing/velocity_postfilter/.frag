#version 330 core

out vec3 FragColor;

in vec2 uv;

uniform vec2 resolution;
uniform sampler2D velocityBuffer;

void main() {
    vec3 bufferSample = texture(velocityBuffer, uv).rgb;
    vec2 velocitySample = bufferSample.rg;

    FragColor = vec3(velocitySample, bufferSample.b);
}
