#version 330 core

out vec4 FragColor;

uniform sampler2D hdrInput;
uniform sampler2D depthInput;
uniform sampler2D velocityInput;

uniform float near;
uniform float far;

uniform float fps;

uniform bool camera;
uniform float cameraIntensity;
uniform int cameraSamples;

uniform bool object;
uniform int objectSamples;

uniform mat4 inverseViewProjectionMatrix;
uniform mat4 previousViewProjectionMatrix;

in vec2 uv;

vec3 getWorldPosition(float depth) {
    // get fragment position in clip space (normalize texture coordinates and depth to NDC)
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    // transform clip space position to world space position
    vec4 worldSpacePosition = inverseViewProjectionMatrix * clipSpacePosition;

    // perspective division
    worldSpacePosition /= worldSpacePosition.w;

    // return world space position
    return worldSpacePosition.xyz;
}

vec4 cameraMotionBlur(vec4 color) {
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
    // scale direction
    blurDirection *= blurScale;

    // perform motion blur on hdr input
    for (int i = 1; i < cameraSamples; ++i) {
        // get blur offset
        vec2 offset = blurDirection * (float(i) / float(cameraSamples - 1) - 0.5) * cameraIntensity;
        // sample iteration
        color += texture(hdrInput, uv + offset);
    }
    // average accumulated samples
    color /= float(cameraSamples);

    // return motion blurred input
    return color;
}

vec4 objectMotionBlur(vec4 color) {
    // get velocity from velocity input
    vec2 velocity = texture(velocityInput, uv).rg;
    // skip further calculations if there is no velocity
    if(velocity == vec2(0.0)) return color;

    // calculate scale for blur direction to compensate varying framerates
    float blurScale = fps / 60;
    // scale velocity
    velocity *= blurScale;

    // perform motion blur on hdr input
    color = texture(hdrInput, uv);
    for(int i = 1; i < objectSamples; ++i){
        // get blur offset
        vec2 offset = velocity * (float(i) / float(objectSamples - 1) - 0.5);
        // sample iteration
        color += texture(hdrInput, uv + offset);
    }
    color /= float(objectSamples);

    // return motion blurred input
    return color;
}

void main() {
    vec4 color = texture(hdrInput, uv);

    // perform camera motion blur
    if (camera) {
        color = cameraMotionBlur(color);
    }

    // perform object motion blur
    if(object){
        color = objectMotionBlur(color);
        // color = vec4(texture(velocityInput, uv).rg, 0.0, 1.0); // Show velocity buffer
    }

    FragColor = color;
}