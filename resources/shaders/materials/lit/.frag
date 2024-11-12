#version 330 core

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 2

out vec4 FragColor;

in vec3 v_normal;
in vec2 v_uv;
in mat3 v_tbnMatrix;
in vec3 v_fragmentWorldPosition;
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

const int NO_FOG = 0;
const int LINEAR_FOG = 1;
const int EXPONENTIAL_FOG = 2;
const int EXPONENTIAL_SQUARED_FOG = 3;

struct Fog {
    int type;
    vec3 color;
    float data[2];
};
uniform Fog fog;

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

    float emissionIntensity;
    vec3 emissionColor;
    bool enableEmissionMap;
    sampler2D emissionMap;
};
uniform Material material;

float sqr(float x)
{
    return x * x;
}

vec3 getNormal() {
    if (!material.enableNormalMap) {
        return normalize(v_normal);
    }

    vec3 normal = texture(material.normalMap, uv).rgb;
    normal = normal * 2.0 - vec3(1.0);
    normal.xy *= material.normalMapIntensity;
    normal = normalize(v_tbnMatrix * normal);
    return normal;
}

float pcf(vec3 projectionCoordinates, float currentDepth, float bias, float smoothing, int kernelRadius)
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
            float pcfDepth = texture(configuration.shadowMap, projectionCoordinates.xy + offset).r;

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

    vec3 normalDirection = getNormal();

    float maxBias = 0.01;
    float minBias = 0.005;
    float bias = max(maxBias * dot(normalDirection, lightDirection), minBias);
    bias = 0.0001f;

    // with pcf
    float shadow = pcf(projectionCoordinates, currentDepth, bias, 1.0, 3);

    // without pcf
    /*float shadowDepth = texture(configuration.shadowMap, projectionCoordinates.xy).r;
        float shadow = currentDepth - bias > shadowDepth ? 1.0 : 0.0;*/

    return shadow;
}

float getLinearFog(float start, float end) {
    float depth = length(v_fragmentWorldPosition - configuration.cameraPosition);
    float fogRange = end - start;
    float fogDistance = end - depth;
    float factor = clamp(fogDistance / fogRange, 0.0, 1.0);
    return factor;
}

float getExponentialFog(float density) {
    float depth = length(v_fragmentWorldPosition - configuration.cameraPosition);
    float factor = 1 / exp(depth * density);
    return factor;
}

float getExponentialSquaredFog(float density) {
    float depth = length(v_fragmentWorldPosition - configuration.cameraPosition);
    float factor = 1 / exp(sqr(depth * density));
    return factor;
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

vec3 getEmission() {
    vec3 emission = vec3(material.emissionIntensity) * material.emissionColor;
    if(material.enableEmissionMap){
        emission *= texture(material.emissionMap, uv).rgb;
    }
    return emission;
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

    vec3 N = getNormal();
    vec3 V = normalize(configuration.cameraPosition - v_fragmentWorldPosition); // view direction

    float dialectricReflecitivity = 0.04;
    vec3 F0 = mix(vec3(dialectricReflecitivity), albedo, metallic); // base reflectivity

    // calculate each lights impact on object
    vec3 Lo = vec3(0.0);

    vec3 emission = getEmission();
    bool hasEmission = length(emission) > 1;

    if (!hasEmission) {
        // fragment doesnt have emission and therefore reflects light from light sources

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

            float distance = length(pointLight.position - v_fragmentWorldPosition);
            float attenuation = getAttenuation_range_falloff_cusp(distance, pointLight.range, pointLight.falloff);
            vec3 L = normalize(pointLight.position - v_fragmentWorldPosition);

            Lo += evaluateLightSource(V, N, F0, roughness, metallic, albedo, attenuation, L, pointLight.color, pointLight.intensity, 0.0);
        }

        // spot lights
        for (int i = 0; i < configuration.numSpotLights; i++) {
            SpotLight spotLight = spotLights[i];

            float distance = length(spotLight.position - v_fragmentWorldPosition);
            float attenuation = getAttenuation_range_falloff_cusp(distance, spotLight.range, spotLight.falloff);
            vec3 L = normalize(spotLight.position - v_fragmentWorldPosition);

            float theta = dot(L, normalize(-spotLight.direction));
            float epsilon = spotLight.innerCutoff - spotLight.outerCutoff;
            float intensityScaling = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0, 1.0);

            Lo += evaluateLightSource(V, N, F0, roughness, metallic, albedo, attenuation, L, spotLight.color, spotLight.intensity * intensityScaling, 0.0);
        }
    } else {
        // fragment has emission and therefore emits light

        Lo = emission;
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

    // get fog
    float fogFactor = 1.0;
    switch (fog.type) {
        case LINEAR_FOG:
        fogFactor = getLinearFog(fog.data[0], fog.data[1]);
        break;
        case EXPONENTIAL_FOG:
        fogFactor = getExponentialFog(fog.data[0]);
        break;
        case EXPONENTIAL_SQUARED_FOG:
        fogFactor = getExponentialSquaredFog(fog.data[0]);
        break;
    }
    color = mix(fog.color, color, fogFactor);

    // return shaded color
    return vec4(color, 1.0);
}

vec4 shadeSolid()
{
    vec3 ambient = ambientLighting.intensity * ambientLighting.color;
    vec3 diffuse = vec3(0.0);

    vec3 N = getNormal();

    for (int i = 0; i < configuration.numDirectionalLights; i++)
    {
        DirectionalLight directionalLight = directionalLights[i];

        float attenuation = 1.0;
        vec3 L = normalize(-directionalLight.direction);

        vec3 shadowDirection = normalize(directionalLight.position - v_fragmentWorldPosition);
        float shadow = getDirectionalShadow(shadowDirection);

        diffuse += max(dot(N, L), 0.0) * directionalLight.color * directionalLight.intensity * attenuation * (1.0 - shadow);
    }

    for (int i = 0; i < configuration.numPointLights; i++)
    {
        PointLight pointLight = pointLights[i];

        float distance = length(pointLight.position - v_fragmentWorldPosition);
        float attenuation = getAttenuation_range_falloff_cusp(distance, pointLight.range, pointLight.falloff);
        vec3 L = normalize(pointLight.position - v_fragmentWorldPosition);

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

vec4 shadeNormal() {
    // get normal
    vec3 normal = getNormal();

    // remap normal range from [-1, 1] to [0, 1]
    normal = (normal + 1.0) * 0.5;

    // if available, sample diffuse lighting and shadow from first directional light for depth enhancement
    vec3 diffuse = vec3(1.0);
    float diffuseFactor = 0.3;
    float shadow = 0.0;
    float shadowIntensity = 0.5;

    for (int i = 0; i < configuration.numDirectionalLights; i++) {
        DirectionalLight directionalLight = directionalLights[i];
        vec3 L = normalize(-directionalLight.direction);

        diffuse = vec3(max(dot(normal, L), 0.0));
        diffuse = mix(diffuse, vec3(0.0), diffuseFactor);

        shadow = getDirectionalShadow(L) * shadowIntensity;

        break;
    }

    // get color from normal and shadow
    vec3 color = normal * diffuse * (1.0 - shadow);

    // return normal as color
    return vec4(color, 1.0);
}

vec4 shadeDepth() {
    // camera clipping default variables
    float near = 0.3;
    float far = 1000;

    // get depth
    float z = gl_FragCoord.z * 2.0 - 1.0;
    float depth = ((2.0 * near * far) / (far + near - z * (far - near))) / far;

    // retun depth as color
    return vec4(vec3(depth), 1.0);
}

vec4 shadeShadowMap() {
    vec3 projectionCoordinates = v_fragmentLightSpacePosition.xyz / v_fragmentLightSpacePosition.w;
    projectionCoordinates = projectionCoordinates * 0.5 + 0.5;
    float depth = texture(configuration.shadowMap, projectionCoordinates.xy).r;
    return vec4(vec3(depth), 1.0);
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
