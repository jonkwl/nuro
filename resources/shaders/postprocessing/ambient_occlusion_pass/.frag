#version 330 core

out vec4 FragColor;

uniform sampler2D depthInput;

uniform vec2 depthResolution;
uniform mat4 inverseViewProjectionMatrix;

in vec2 uv;

vec3 normal;

vec3 getWorldPosition(float depth, vec2 uvSample) {
    // get fragment position in clip space (convert texture coordinates and depth to NDC)
    vec4 clipSpacePosition = vec4(uvSample * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    // transform clip space position to world space position
    vec4 worldSpacePosition = inverseViewProjectionMatrix * clipSpacePosition;

    // perspective division
    worldSpacePosition /= worldSpacePosition.w;

    // return world space position
    return vec3(worldSpacePosition);
}

vec3 getNormal() {
    // Get uv samples
    vec2 uv_1 = uv;
    vec2 uv_2 = uv + vec2(1, 0) / depthResolution;
    vec2 uv_3 = uv + vec2(0, 1) / depthResolution;

    // Get depth samples
    float depth_1 = texture(depthInput, uv_1).r;
    float depth_2 = texture(depthInput, uv_2).r;
    float depth_3 = texture(depthInput, uv_3).r;

    // Get world positions
    vec3 worldPos_1 = getWorldPosition(depth_1, uv_1);
    vec3 worldPos_2 = getWorldPosition(depth_2, uv_2);
    vec3 worldPos_3 = getWorldPosition(depth_3, uv_3);

    // Calculate normals
    return normalize(cross(worldPos_3 - worldPos_1, worldPos_2 - worldPos_1));
}

void main()
{
    // calculate fragment normal
    normal = getNormal();

    // shade normals
    FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}
