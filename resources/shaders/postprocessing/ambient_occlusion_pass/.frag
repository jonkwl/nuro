#version 330 core

out vec4 FragColor;

uniform sampler2D depthInput;
uniform sampler2D normalInput;
uniform sampler2D noiseTexture;
uniform float noiseSize;

#define N_MAX_SAMPLES 64
uniform vec3 samples[N_MAX_SAMPLES];

uniform vec2 resolution;
uniform mat4 projectionMatrix;
uniform mat4 inverseProjectionMatrix;

in vec2 uv;

vec3 normal;
vec3 viewPosition;
vec2 noiseScale;
vec3 noiseSample;

int nSamples = 64;
float radius = 0.5;
// float bias = 0.025;
float bias = 0.0;
float power = 2.0;

vec3 getViewPosition() {
    // sample current fragments depth
    float depth = texture(depthInput, uv).r;

    // get fragment position in clip space (convert texture coordinates and depth to NDC)
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    // transform clip space position to view space position
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;

    // perspective division
    viewSpacePosition /= viewSpacePosition.w;

    // return view space position
    return vec3(viewSpacePosition);
}

vec3 decodeNormalInput() {
    // get normal sample from normal input
    vec3 normalSample = texture(normalInput, uv).rgb;

    // remap from [0, 1] to [-1, 1]
    return normalize(normalSample * 2.0 - 1.0);
}

float calculateOcclusion(){
    // calculate tangen and bitangent for noise sample
    vec3 tangent = normalize(noiseSample - normal * dot(noiseSample, normal));
    vec3 bitangent = cross(normal, tangent);

    // get tbn matrix from tangent, bitangen and normal
    mat3 tbn = mat3(tangent, bitangent, normal);

    // occlusion factor
    float occlusion = 0.0;

    // loop through each sample
    for(int i = 0; i < nSamples; ++i){
        // get sample position in view space
        vec3 samplePosition = tbn * samples[i];

        // set sample position to fragments view position offset by sample position and multiply with radius
        samplePosition = viewPosition + samplePosition * radius;

        // get samples position in screen space
        vec4 viewSamplePosition = vec4(samplePosition, 1.0); // set to sample position as vec4
        viewSamplePosition = projectionMatrix * viewSamplePosition; // transform to clip space
        viewSamplePosition.xy /= viewSamplePosition.w; // perspective division
        viewSamplePosition.xy = viewSamplePosition.xy * 0.5 + 0.5; // transform to range [0 - 1]

        // calculate depth sample of sample position in view space
        float depthSample = texture(depthInput, viewSamplePosition.xy).r;
        vec4 viewDepth = vec4(0.0, 0.0, depthSample * 2.0 - 1.0, 1.0);
        viewDepth = inverseProjectionMatrix * viewDepth;
        viewDepth.xyz /= viewDepth.w;
        depthSample = viewDepth.z;

        // perform range check and add to occlusion
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPosition.z - depthSample));
        occlusion += (depthSample >= samplePosition.z + bias ? 1.0 : 0.0) * rangeCheck; 
    }

    // get occlusion value
    occlusion = 1.0 - (occlusion / nSamples);

    // apply power to occlusion
    occlusion = pow(occlusion, power);

    // return occlusion
    return occlusion;
}

void main()
{
    // get fragment normal
    normal = decodeNormalInput();

    // get fragment view position
    viewPosition = getViewPosition();

    // calculate noise scale
    noiseScale = vec2(resolution.x / noiseSize, resolution.y / noiseSize);

    // get sample from noise texture
    noiseSample = texture(noiseTexture, uv * noiseScale).xyz;

    // calculate occlusion factor
    float occlusion = calculateOcclusion();

    FragColor = vec4(vec3(occlusion), 1.0);
}