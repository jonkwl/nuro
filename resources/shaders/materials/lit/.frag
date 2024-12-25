#version 330 core

#define PI 3.14159265359

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 15
#define MAX_SPOT_LIGHTS 2

out vec4 FragColor;

in vec3 v_normal;
in vec2 v_uv;
in mat3 v_tbnMatrix;
in vec3 v_fragmentWorldPosition;
in vec4 v_fragmentLightSpacePosition;

vec2 uv;
vec2 viewportUv;
vec3 normal;

struct Configuration {
    // General parameters
    float gamma;
    bool solidMode;
    vec2 viewportResolution;

    // Shadow parameters
    bool castShadows;
    sampler2D shadowMap;
    float shadowMapResolutionWidth;
    float shadowMapResolutionHeight;

    sampler3D shadowDisk;
    float shadowDiskWindowSize;
    float shadowDiskFilterSize;
    float shadowDiskRadius;

    // World parameters
    vec3 cameraPosition;

    // Lighting parameters
    int numDirectionalLights;
    int numPointLights;
    int numSpotLights;

    // SSAO
    bool enableSSAO;
    sampler2D ssaoBuffer;
};
uniform Configuration configuration;

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

#define NO_FOG 0
#define LINEAR_FOG 1
#define EXPONENTIAL_FOG 2
#define EXPONENTIAL_SQUARED_FOG 3

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

    float roughness;
    bool enableRoughnessMap;
    sampler2D roughnessMap;

    float metallic;
    bool enableMetallicMap;
    sampler2D metallicMap;

    bool enableNormalMap;
    sampler2D normalMap;
    float normalMapIntensity;

    bool enableOcclusionMap;
    sampler2D occlusionMap;

    bool emission;
    float emissionIntensity;
    vec3 emissionColor;
    bool enableEmissiveMap;
    sampler2D emissiveMap;

    bool enableHeightMap;
    sampler2D heightMap;
    float heightMapScale;
};
uniform Material material;

float sqr(float x)
{
    return x * x;
}

vec3 getShadowCoords() {
    vec3 projectionCoords = v_fragmentLightSpacePosition.xyz / v_fragmentLightSpacePosition.w;
    vec3 shadowCoords = projectionCoords * 0.5 + vec3(0.5);
    return shadowCoords;
}

float getDirectionalShadowBias(vec3 lightDirection) {
    /* float diffuseFactor = dot(normal, -lightDirection);
                        float bias = mix(0.0001, 0.0, diffuseFactor); */
    float bias = 0.0;
    return bias;
}

float getDirectionalHardShadow(vec3 lightDirection)
{
    // make sure shadows are enabled
    if (!configuration.castShadows) {
        return 0.0;
    }

    // get shadow coordinates
    vec3 shadowCoords = getShadowCoords();

    // if shadow coordinate's depth is beyond 1.0, fragment isn't in shadow
    if (shadowCoords.z > 1.0) return 0.0;

    // check if fragment is in shadow
    float depth = texture(configuration.shadowMap, shadowCoords.xy).r;
    float bias = getDirectionalShadowBias(lightDirection);
    float shadow = shadowCoords.z - bias > depth ? 1.0 : 0.0;

    // return shadow value
    return shadow;
}

float getDirectionalSoftShadow(vec3 lightDirection)
{
    // make sure shadows are enabled
    if (!configuration.castShadows) {
        return 0.0;
    }

    // get shadow coordinates
    vec3 shadowCoords = getShadowCoords();

    // if shadow coordinate's depth is beyond 1.0, fragment isn't in shadow
    if (shadowCoords.z > 1.0) return 0.0;

    // initialize offset for sampling shadow map at different positions
    ivec3 offsetCoord;

    // get fractional part of fragment's screen position (for sampling)
    vec2 f = mod(gl_FragCoord.xy, vec2(configuration.shadowDiskWindowSize));

    // assign fractional part to y and z components of offset
    offsetCoord.yz = ivec2(f);

    // create a vec4 for shadow coordinates with a z of 1.0 for sampling
    vec4 sc = vec4(shadowCoords, 1.0);

    // initialize sum for accumulated shadow result
    float sum = 0.0;

    // calculate number of samples to take based on filter size
    int samplesDiv2 = int(configuration.shadowDiskFilterSize * configuration.shadowDiskFilterSize / 2.0);

    // calculate texel size for shadow map based on its dimensions
    float texelWidth = 1.0 / configuration.shadowMapResolutionWidth;
    float texelHeight = 1.0 / configuration.shadowMapResolutionHeight;

    // store texel size in a vec2
    vec2 texelSize = vec2(texelWidth, texelHeight);

    // calculate a small bias to prevent self-shadowing artifacts
    float bias = getDirectionalShadowBias(lightDirection);
    float depth = 0.0;

    // loop through a small number of samples in 2x2 pattern (8 total)
    for (int i = 0; i < 4; i++) {
        offsetCoord.x = i; // set x offset for this sample

        // fetch offsets from shadow disk texture, scaled by shadow radius
        vec4 Offsets = texelFetch(configuration.shadowDisk, offsetCoord, 0) * configuration.shadowDiskRadius;

        // sample shadow map at first offset location
        sc.xy = shadowCoords.xy + Offsets.rg * texelSize;
        depth = texture(configuration.shadowMap, sc.xy).x;

        // compare depth to shadow coordinate z value to determine if in shadow
        shadowCoords.z - bias > depth ? sum += 1.0 : sum += 0.0;

        // sample shadow map at second offset location
        sc.xy = shadowCoords.xy + Offsets.ba * texelSize;
        depth = texture(configuration.shadowMap, sc.xy).x;

        // compare depth again
        shadowCoords.z - bias > depth ? sum += 1.0 : sum += 0.0;
    }

    // compute shadow result as average of 8 samples
    float shadow = sum / 8.0;

    // if shadow is not fully black or fully white, we need more samples for softening
    if (shadow != 0.0 && shadow != 1.0) {
        // loop through additional samples (beyond initial 8)
        for (int i = 4; i < samplesDiv2; i++) {
            offsetCoord.x = i;

            // fetch more offsets from shadow disk texture
            vec4 Offsets = texelFetch(configuration.shadowDisk, offsetCoord, 0) * configuration.shadowDiskRadius;

            // sample shadow map at first offset location
            sc.xy = shadowCoords.xy + Offsets.rg * texelSize;
            depth = texture(configuration.shadowMap, sc.xy).x;

            // compare depth to shadow coordinate z value
            shadowCoords.z - bias > depth ? sum += 1.0 : sum += 0.0;

            // sample at second offset location
            sc.xy = shadowCoords.xy + Offsets.ba * texelSize;
            depth = texture(configuration.shadowMap, sc.xy).x;

            // compare depth again
            shadowCoords.z - bias > depth ? sum += 1.0 : sum += 0.0;
        }

        // calculate final shadow result as average of all samples
        shadow = sum / float(samplesDiv2 * 2.0);
    }

    // return shadow value
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

vec2 transformUvByHeightmap(vec2 inputUv) {
    // Calculate view direction
    vec3 viewDirection = v_tbnMatrix * normalize(configuration.cameraPosition - v_fragmentWorldPosition);

    // Calculate layer depth
	float minLayers = 8.0;
    float maxLayers = 64.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirection)));
	float layerDepth = 1.0 / numLayers;
	
    // Current layer depth cache
    float currentLayerDepth = 0.0;
	
	// Calculate delta uv
	vec2 P = viewDirection.xy / viewDirection.z * material.heightMapScale; 
    vec2 deltaUv = P / numLayers;

    // Initialize output uvs with input
    vec2 outputUv = inputUv;
	
    // Get height sample
	float heightSample = texture(material.heightMap, outputUv).r;
	
	// Loop until layer sample depth is smaller than heightmap sample depth
	while(currentLayerDepth < heightSample)
    {
        outputUv -= deltaUv;
        heightSample = texture(material.heightMap, outputUv).r;
        currentLayerDepth += layerDepth;
    }

	// Apply occlusion
	vec2 previousUv = outputUv + deltaUv;
	float depthAfter  = heightSample - currentLayerDepth;
	float depthBefore = texture(material.heightMap, previousUv).r - currentLayerDepth + layerDepth;
	float weight = depthAfter / (depthAfter - depthBefore);
	outputUv = previousUv * weight + outputUv * (1.0 - weight);

    // Ensure output uvs are in range
	if(outputUv.x > 1.0 || outputUv.y > 1.0 || outputUv.x < 0.0 || outputUv.y < 0.0) discard;

    return outputUv;
}

vec2 getUv() {
    // calculate scaled texture coordinates by material properties
    vec2 _uv = v_uv * material.tiling + material.offset;

    // height map enabled, transform texture coordinates by heightmap
    if (material.enableHeightMap) {
        _uv = transformUvByHeightmap(_uv);
    }

    // return texture coordinates
    return _uv;
}

vec3 getNormal() {
    // no normal mapping, return input normal
    if (!material.enableNormalMap) {
        return v_normal;
    }

    // normal mapping enabled

    // sample normal map
    vec3 N = texture(material.normalMap, uv).rgb;

    // normalize sampled normal
    N = N * 2.0 - vec3(1.0);

    // scale normal x and y by normal map intensity
    N.xy *= material.normalMapIntensity;

    // transform normal into world space
    N = normalize(v_tbnMatrix * N);

    // return normal
    return N;
}

vec3 getAlbedo()
{
    // default albedo is white
    vec3 albedo = vec3(1.0);

    // sample albedo map if enabled
    if (material.enableAlbedoMap) {
        vec3 albedoSample = texture(material.albedoMap, uv).rgb;
        albedo = pow(albedoSample, vec3(configuration.gamma));
    }

    // tint albedo by materials base color
    albedo *= vec3(material.baseColor);

    // return final albedo
    return albedo;
}

float getRoughness()
{
    // initialize roughness
    float roughness = 0.0;

    // roughness map enabled, sample roughness by roughness map
    if (material.enableRoughnessMap) {
        roughness = texture(material.roughnessMap, uv).r;
        // no roughness map, set to materials roughness property
    } else {
        roughness = material.roughness;
    }

    // return roughness
    return roughness;
}

float getMetallic()
{
    // initialize metallic
    float metallic = 0.0;

    // metallic map enabled, sample metallic by metallic map
    if (material.enableMetallicMap) {
        metallic = texture(material.metallicMap, uv).r;
        // no metallic map, set to materials metallic property
    } else {
        metallic = material.metallic;
    }

    // return metallic
    return metallic;
}

float getOcclusionMapSample()
{
    // initialize occlusion map sample with no occlusion
    float occlusionMapSample = 1.0;

    // occlusion map enabled, sample by occlusion map
    if (material.enableOcclusionMap) {
        occlusionMapSample = texture(material.occlusionMap, uv).r;
    }

    // return occlusion map sample
    return occlusionMapSample;
}

float getSSAO() {
    // initialize ssao sample with no occlusion
    float ssao = 1.0;

    // ssao enabled, sample by ssao buffer
    if (configuration.enableSSAO) {
        ssao = texture(configuration.ssaoBuffer, viewportUv).r;
    }

    // return ssao sample
    return ssao;
}

vec3 getEmission() {
    // get emission by intensity and color
    vec3 emission = vec3(material.emissionIntensity) * material.emissionColor;

    // emissive map enabled, tint emission by emissive map sample
    if (material.enableEmissiveMap) {
        emission *= texture(material.emissiveMap, uv).rgb;
    }

    // return emission
    return emission;
}

vec3 evaluateLightSource(
    vec3 V,
    vec3 N,
    vec3 F0,
    float roughness,
    float metallic,
    vec3 albedo,
    float attenuation,
    vec3 L,
    vec3 color,
    float intensity,
    float shadow) {

    // fragment is fully in light sources shadow, no light source contribution
    if (shadow == 1.0) {
        return vec3(0.0);
    }

    // halfway direction
    vec3 H = normalize(V + L);

    // cosine of angle between N and L, indicates effective light contribution from source
    float NdotL = max(dot(N, L), 0.0);

    // no light contribution from source, no light source contribution
    if (NdotL == 0.0)
    {
        return vec3(0.0);
    }

    // no light contribution from source, no light source contribution
    if (max(attenuation, 0.0) == 0.0)
    {
        return vec3(0.0);
    }

    // per-light radiance
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
    if (s >= 1.0) {
        return 0.0;
    }
    float s2 = sqr(s);
    return sqr(1 - s2) / (1 + falloff * s2);
}

float getAttenuation_range_falloff_cusp(float distance, float range, float falloff)
{
    float s = distance / range;
    if (s >= 1.0) {
        return 0.0;
    }
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
    float occlusionMapSample = getOcclusionMapSample();

    // get ssao
    float ssao = getSSAO();

    vec3 N = normal;
    vec3 V = normalize(configuration.cameraPosition - v_fragmentWorldPosition); // view direction

    float dialectricReflecitivity = 0.04;
    vec3 F0 = mix(vec3(dialectricReflecitivity), albedo, metallic); // base reflectivity

    // final light reflection or emission of fragment
    vec3 Lo = vec3(0.0);

    // get emission and check if fragment has emission
    vec3 emission = getEmission();

    // fragment has emission and therefore emits light
    if (material.emission || material.enableEmissiveMap) {
        Lo = emission;
    }

    // fragment doesnt have emission and therefore reflects light from light sources
    else {

        // directional lights
        for (int i = 0; i < configuration.numDirectionalLights; i++)
        {
            DirectionalLight directionalLight = directionalLights[i];

            float attenuation = 1.0;
            vec3 L = normalize(-directionalLight.direction);

            float shadow = getDirectionalSoftShadow(L);

            Lo += evaluateLightSource(
                    V,
                    N,
                    F0,
                    roughness,
                    metallic,
                    albedo,
                    attenuation,
                    L,
                    directionalLight.color,
                    directionalLight.intensity,
                    shadow);
        }

        // point lights
        for (int i = 0; i < configuration.numPointLights; i++) {
            PointLight pointLight = pointLights[i];

            float distance = length(pointLight.position - v_fragmentWorldPosition);
            float attenuation = getAttenuation_range_falloff_cusp(distance, pointLight.range, pointLight.falloff);
            vec3 L = normalize(pointLight.position - v_fragmentWorldPosition);

            Lo += evaluateLightSource(
                    V,
                    N,
                    F0,
                    roughness,
                    metallic,
                    albedo,
                    attenuation,
                    L,
                    pointLight.color,
                    pointLight.intensity,
                    0.0);
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

            Lo += evaluateLightSource(
                    V,
                    N,
                    F0,
                    roughness,
                    metallic,
                    albedo,
                    attenuation,
                    L,
                    spotLight.color,
                    spotLight.intensity * intensityScaling,
                    0.0);
        }
    }

    // assemble shaded color
    vec3 color = Lo;

    // apply ambient occlusion: modulate color by ambient occlusion map sample and ssao
    color *= occlusionMapSample * ssao;

    // gamma correct if using albedo map
    if (material.enableAlbedoMap) {
        color = pow(color, vec3(1.0 / configuration.gamma));
    }

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
    vec3 diffuse = vec3(0.0);

    vec3 N = normal;

    for (int i = 0; i < configuration.numDirectionalLights; i++)
    {
        DirectionalLight directionalLight = directionalLights[i];

        float attenuation = 1.0;
        vec3 L = normalize(-directionalLight.direction);

        vec3 shadowDirection = normalize(directionalLight.position - v_fragmentWorldPosition);
        float shadow = getDirectionalHardShadow(shadowDirection);

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

    vec3 color = diffuse * albedo;

    return vec4(color, 1.0);
}

vec4 shadeNormal() {
    // get normal
    vec3 colorNormal = normal;

    // remap normal range from [-1, 1] to [0, 1]
    colorNormal = (colorNormal + 1.0) * 0.5;

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

        shadow = getDirectionalHardShadow(L) * shadowIntensity;

        break;
    }

    // get color from normal and shadow
    vec3 color = colorNormal * diffuse * (1.0 - shadow);

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
    viewportUv = gl_FragCoord.xy / vec2(configuration.viewportResolution.x, configuration.viewportResolution.y);
    uv = getUv();
    normal = getNormal();

    if (!configuration.solidMode) {
        FragColor = shadePBR();
    } else {
        FragColor = shadeSolid();
    }
}
