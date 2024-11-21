#version 330 core

out vec3 FragColor;

uniform sampler2D hdrInput;
uniform sampler2D depthInput;

uniform float near;
uniform float far;

uniform float fps;

uniform float intensity;
uniform int nSamples;

uniform mat4 previousViewProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 inverseProjectionMatrix;

in vec2 uv;

float linearizeDepth(float depth) {
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

vec3 getWorldPosition(float depth) {
    // get fragment position in clip space (normalize texture coordinates and depth to NDC)
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    // transform clip space position to view space position
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;

    // perspective division
    viewSpacePosition /= viewSpacePosition.w;

    // transform view space position to world space position
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;

    // return world space position
    return worldSpacePosition.xyz;
}

vec3 cameraMotionBlur(vec3 color) {
    // get fragments current position in world space
    vec3 currentWorldPosition = getWorldPosition(texture(depthInput, uv).r);

    // get fragments previous position in screen space
    vec4 previousScreenPosition = previousViewProjectionMatrix * vec4(currentWorldPosition, 1.0);
    previousScreenPosition.xyz /= previousScreenPosition.w;
    previousScreenPosition.xy = previousScreenPosition.xy * 0.5 + 0.5;

    // calculate scale for blur direction to compensate varying framerates
    float blurScale = fps / 60;

    // calculate direction for motion blur
    vec2 blurDirection = previousScreenPosition.xy - uv;
    blurDirection *= blurScale;

    // perform motion blur on input
    for (int i = 1; i < nSamples; ++i) {
        // get blur offset
        vec2 offset = blurDirection * (float(i) / float(nSamples - 1) - 0.5) * intensity;
        // sample iteration
        color += texture(hdrInput, uv + offset).rgb;
    }
    // average accumulated samples to motion blur input
    color /= float(nSamples);

    // return motion blurred input
    return color;
}

void main() {
    vec3 color = texture(hdrInput, uv).rgb;

    // perform camera motion blur
    color = cameraMotionBlur(color);

    FragColor = color;
}
