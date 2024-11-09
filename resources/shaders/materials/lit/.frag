#version 460 core

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 2

out vec4 FragColor;

in vec3 v_normal;
in vec2 v_uv;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec3 v_fragmentPosition;
in vec4 v_fragmentLightSpacePosition;

const float PI = 3.14159265359;

vec2 uv;

struct Configuration {
    float gamma;

    bool solidMode;
    bool castShadows;

    sampler2D shadowMap;
    vec3 cameraPosition;

    int numDirectionalLights;
    int numPointLights;
    int numSpotLights;
};
uniform Configuration configuration;

struct AmbientLighting {
    float intensity;
    vec3 color;
};
uniform AmbientLighting ambientLighting;

struct DirectionalLight {
    float intensity;
    vec3 direction;
    vec3 color;
    vec3 position; // boilerplate for directional shadows
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float falloff;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float falloff;
    float innerCutoff;
    float outerCutoff;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

struct Material {
    vec4 baseColor;

    vec2 tiling;
    vec2 offset;

    bool enableAlbedoMap;
    sampler2D albedoMap;

    bool enableNormalMap;
    sampler2D normalMap;
    float normalMapIntensity;

    float roughness;
    bool enableRoughnessMap;
    sampler2D roughnessMap;

    float metallic;
    bool enableMetallicMap;
    sampler2D metallicMap;

    bool enableAmbientOcclusionMap;
    sampler2D ambientOcclusionMap;
};
uniform Material material;

float sqr(float x)
{
    return x * x;
}

vec3 getNormals(){
    if (!material.enableNormalMap){
        return normalize(v_normal);
    }

    vec3 normalSample = texture(material.normalMap, uv).rgb;
    normalSample = normalSample * 2.0 - vec3(1.0);
    normalSample.xy *= material.normalMapIntensity;
    normalSample = normalize(normalSample);

    vec3 tangent = normalize(v_tangent);
    vec3 bitangent = normalize(v_bitangent);
    vec3 normal = normalize(v_normal);
    mat3 TBN = mat3(tangent, bitangent, normal);

    return normalize(TBN * normalSample);
}

float pcf(vec3 projectionCoords, float currentDepth, float bias, float smoothing, int kernelRadius)
{
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(configuration.shadowMap, 0); // Pre-calculate texel size once

    int sampleCount = 0;
    float weightSum = 0.0;

    // Precompute the smoothing factor as it doesn't change during iteration
    float kernelRadiusSquared = float(kernelRadius) * float(kernelRadius);

    // Loop over the kernel area
    for (int x = -kernelRadius; x <= kernelRadius; ++x) {
        for (int y = -kernelRadius; y <= kernelRadius; ++y) {

            // Calculate the offset for the sample
            vec2 offset = vec2(float(x), float(y)) * texelSize * smoothing;

            // Fetch the depth from the shadow map
            float pcfDepth = texture(configuration.shadowMap, projectionCoords.xy + offset).r;

            // Calculate the distance from the center of the kernel and apply Gaussian weight
            float distSquared = float(x * x + y * y);
            if (distSquared > kernelRadiusSquared) continue; // Skip unnecessary samples outside the kernel radius

            float weight = exp(-distSquared / (2.0 * kernelRadiusSquared));

            // Accumulate shadow result (use conditional logic to determine if it's shadowed)
            if (currentDepth - bias > pcfDepth) {
                shadow += weight;
            }

            // Accumulate weight sum
            weightSum += weight;
            sampleCount++;
        }
    }

    // Normalize the shadow result by the total weight sum
    if (sampleCount > 0) {
        shadow = shadow / weightSum;
    }

    return shadow;
}

float getDirectionalShadow(vec3 lightDirection)
{
    if (!configuration.castShadows) {
        return 0.0f;
    }

    vec3 projectionCoordinates = v_fragmentLightSpacePosition.xyz / v_fragmentLightSpacePosition.w;
    projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

    if (projectionCoordinates.z > 1.0) return 0.0f;

    float closestDepth = texture(configuration.shadowMap, projectionCoordinates.xy).r;
    float currentDepth = projectionCoordinates.z;

    vec3 normalDirection = getNormals();

    float maxBias = 0.01;
    float minBias = 0.005;
    float bias = max(maxBias * dot(normalDirection, lightDirection), minBias);
    bias = 0.0001f;

    float shadow = pcf(projectionCoordinates, currentDepth, bias, 1.0, 3);

    return shadow;
}

vec3 fresnelSchlick(float VdotN, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotN, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;
    return numerator / denominator;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float numerator = NdotV;
    float denominator = NdotV * (1.0 - k) + k;
    return numerator / denominator;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 getAlbedo()
{
    vec3 albedo = vec3(1.0);
    if (material.enableAlbedoMap) {
        albedo = pow(texture(material.albedoMap, uv).rgb, vec3(configuration.gamma));
    }
    albedo *= vec3(material.baseColor);
    return albedo;
}

float getRoughness()
{
    float roughness = 0.0;
    if (material.enableRoughnessMap) {
        roughness = texture(material.roughnessMap, uv).r;
    } else {
        roughness = material.roughness;
    }
    return roughness;
}

float getMetallic()
{
    float metallic = 0.0;
    if (material.enableMetallicMap) {
        metallic = texture(material.metallicMap, uv).r;
    } else {
        metallic = material.metallic;
    }
    return metallic;
}

float getAmbientOcclusion()
{
    float ambientOcclusion = 1.0;
    if (material.enableAmbientOcclusionMap) {
        ambientOcclusion = texture(material.ambientOcclusionMap, uv).r;
    }
    return ambientOcclusion;
}

vec3 getAmbient(vec3 albedo, float ambientOcclusion) {
    vec3 ambient = vec3(ambientLighting.intensity) * ambientLighting.color * albedo * ambientOcclusion;
    return ambient;
}

vec3 evaluateLightSource(vec3 V, vec3 N, vec3 F0, float roughness, float metallic, vec3 albedo, float attenuation, vec3 L, vec3 color, float intensity, float shadow) {
    vec3 H = normalize(V + L); // halfway direction

    // cosine of angle between N and L, indicates effective light contribution from source
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL == 0.0)
    {
        return vec3(0.0); // no light contribution from source, skip light
    }

    // per-light radiance
    if (attenuation == 0.0)
    {
        return vec3(0.0); // no light contribution from source, skip light
    }
    vec3 radiance = color * intensity * attenuation;

    // specular component
    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float epsilon = 0.0001;
    vec3 specular = numerator / max(denominator, epsilon);

    // diffuse component (lambertian)
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;
    vec3 diffuse = kD * albedo / PI;

    // return light source contribution
    vec3 contribution = (diffuse + specular) * radiance * NdotL;
    contribution *= (1.0 - shadow);
    return contribution;
}

float getAttenuation_linear_quadratic(float distance, float linear, float quadratic)
{
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * (distance * distance));
    return attenuation;
}

float getAttenuation_range_infinite(float distance, float range)
{
    float attenuation = 1.0 / (1.0 + sqr((distance / range)));
    return attenuation;
}

float getAttenuation_range_falloff_no_cusp(float distance, float range, float falloff)
{
    float s = distance / range;
    if (s >= 1.0)
        return 0.0;
    float s2 = sqr(s);
    return sqr(1 - s2) / (1 + falloff * s2);
}

float getAttenuation_range_falloff_cusp(float distance, float range, float falloff)
{
    float s = distance / range;
    if (s >= 1.0)
        return 0.0;
    float s2 = sqr(s);
    return sqr(1 - s2) / (1 + falloff * s);
}

vec4 shadePBR() {
    // get albedo
    vec3 albedo = getAlbedo();

    // get roughness
    float roughness = getRoughness();

    // get metallic
    float metallic = getMetallic();

    // get ambient occlusion
    float ambientOcclusion = getAmbientOcclusion();

    vec3 N = getNormals();
    vec3 V = normalize(configuration.cameraPosition - v_fragmentPosition); // view direction

    float dialectricReflecitivity = 0.04;
    vec3 F0 = mix(vec3(dialectricReflecitivity), albedo, metallic); // base reflectivity

    // calculate each lights impact on object
    vec3 Lo = vec3(0.0);

    // directional lights
    for (int i = 0; i < configuration.numDirectionalLights; i++)
    {
        DirectionalLight directionalLight = directionalLights[i];

        float attenuation = 1.0;
        vec3 L = normalize(-directionalLight.direction);

        float shadow = getDirectionalShadow(L);

        Lo += evaluateLightSource(V, N, F0, roughness, metallic, albedo, attenuation, L, directionalLight.color, directionalLight.intensity, shadow);
    }

    // point lights
    for (int i = 0; i < configuration.numPointLights; i++) {
        PointLight pointLight = pointLights[i];

        float distance = length(pointLight.position - v_fragmentPosition);
        float attenuation = getAttenuation_range_falloff_cusp(distance, pointLight.range, pointLight.falloff);
        vec3 L = normalize(pointLight.position - v_fragmentPosition);

        Lo += evaluateLightSource(V, N, F0, roughness, metallic, albedo, attenuation, L, pointLight.color, pointLight.intensity, 0.0);
    }

    // spot lights
    for (int i = 0; i < configuration.numSpotLights; i++) {
        SpotLight spotLight = spotLights[i];

        float distance = length(spotLight.position - v_fragmentPosition);
        float attenuation = getAttenuation_range_falloff_cusp(distance, spotLight.range, spotLight.falloff);
        vec3 L = normalize(spotLight.position - v_fragmentPosition);

        float theta = dot(L, normalize(-spotLight.direction));
        float epsilon = spotLight.innerCutoff - spotLight.outerCutoff;
        float intensityScaling = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0, 1.0);

        Lo += evaluateLightSource(V, N, F0, roughness, metallic, albedo, attenuation, L, spotLight.color, spotLight.intensity * intensityScaling, 0.0);
    }

    // assemble shaded color
    vec3 color = Lo;

    // gamma correct if using albedo map
    if (material.enableAlbedoMap) {
        color = pow(color, vec3(1.0 / configuration.gamma));
    }

    // get ambient
    vec3 ambient = getAmbient(albedo, ambientOcclusion);
    color += ambient;

    // return shaded color
    return vec4(color, 1.0);
}

vec4 shadeSolid()
{
    vec3 ambient = ambientLighting.intensity * ambientLighting.color;
    vec3 diffuse = vec3(0.0);

    vec3 N = getNormals();

    for (int i = 0; i < configuration.numDirectionalLights; i++)
    {
        DirectionalLight directionalLight = directionalLights[i];

        float attenuation = 1.0;
        vec3 L = normalize(-directionalLight.direction);

        vec3 shadowDirection = normalize(directionalLight.position - v_fragmentPosition);
        float shadow = getDirectionalShadow(shadowDirection);

        diffuse += max(dot(N, L), 0.0) * directionalLight.color * directionalLight.intensity * attenuation * (1.0 - shadow);
    }

    for (int i = 0; i < configuration.numPointLights; i++)
    {
        PointLight pointLight = pointLights[i];

        float distance = length(pointLight.position - v_fragmentPosition);
        float attenuation = getAttenuation_range_falloff_cusp(distance, pointLight.range, pointLight.falloff);
        vec3 L = normalize(pointLight.position - v_fragmentPosition);

        float shadow = 0.0;

        diffuse += max(dot(N, L), 0.0) * pointLight.color * pointLight.intensity * attenuation * (1.0 - shadow);
    }

    vec3 albedo = vec3(1.0);
    if (material.enableAlbedoMap) {
        albedo = texture(material.albedoMap, uv).rgb;
    }
    albedo *= vec3(material.baseColor);

    vec3 color = (ambient + diffuse) * albedo;

    return vec4(color, 1.0);
}

vec3 InverseACES(vec3 y) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    vec3 x = y;
    for (int i = 0; i < 5; ++i) {
        vec3 num = y * (x * (c * x + d) + e);
        vec3 den = x * (a * x + b);
        vec3 f_x = num / den - 1.0;
        x = x - f_x / (a * x + b);
    }
    return clamp(x, 0.0, 1.0);
}

vec4 shadeNormals() {
    vec3 normals = getNormals();
    return vec4(normals, 1.0);
}

void main()
{
    uv = v_uv * material.tiling + material.offset;
    if (!configuration.solidMode) {
        FragColor = shadePBR();
    } else {
        FragColor = shadeSolid();
    }
}