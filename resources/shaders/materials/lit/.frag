#version 330 core

out vec4 FragColor;

in vec3 v_normals;
in vec3 v_fragmentPosition;
in vec4 v_fragmentLightSpacePosition;
in vec2 v_textureCoords;

const float gamma = 2.2;
const float PI = 3.14159265359;

vec2 uv;

struct Scene {
    sampler2D shadowMap;
    vec3 cameraPosition;

    vec3 ambientColor;
    float ambientStrength;
};
uniform Scene scene;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};
uniform Light light;

struct Material {
    vec4 baseColor;

    vec2 tiling;
    vec2 offset;

    bool enableAlbedoMap;
    sampler2D albedoMap;

    bool enableNormalMap;
    sampler2D normalMap;

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

float pcf(vec3 projectionCoords, float currentDepth, float bias, float smoothing, int kernelRadius) 
{
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(scene.shadowMap, 0);
    
    int sampleCount = 0;
    float weightSum = 0.0;

    for (int x = -kernelRadius; x <= kernelRadius; ++x) {
        for (int y = -kernelRadius; y <= kernelRadius; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize * smoothing;
            float pcfDepth = texture(scene.shadowMap, projectionCoords.xy + offset).r;
            float dist = length(vec2(x, y));
            float weight = exp(-dist * dist / (2.0 * float(kernelRadius) * float(kernelRadius)));
            shadow += weight * (currentDepth - bias > pcfDepth ? 1.0 : 0.0);
            weightSum += weight;
            sampleCount++;
        }
    }

    shadow = shadow / weightSum;
    
    return shadow;
}

float getShadow()
{
    vec3 projectionCoordinates = v_fragmentLightSpacePosition.xyz / v_fragmentLightSpacePosition.w;
    projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

    if(projectionCoordinates.z > 1.0) return 0.0f;

    float closestDepth = texture(scene.shadowMap, projectionCoordinates.xy).r;
    float currentDepth = projectionCoordinates.z;

    vec3 normalDirection = normalize(v_normals);
    vec3 lightDirection = normalize(light.position - v_fragmentPosition);

    float maxBias = 0.01;
    float minBias = 0.005;
    float bias = max(maxBias * dot(normalDirection, lightDirection), minBias);
    bias = 0.001f;

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
    if(material.enableAlbedoMap){
        albedo = pow(texture(material.albedoMap, uv).rgb, vec3(gamma));
    }
    albedo *= vec3(material.baseColor);
    return albedo;
}

float getRoughness() 
{
    float roughness = 0.0;
    if(material.enableRoughnessMap){
        roughness = texture(material.roughnessMap, uv).r;
    } else{
        roughness = material.roughness;
    }
    return roughness;
}

float getMetallic() 
{
    float metallic = 0.0;
    if(material.enableMetallicMap){
        metallic = texture(material.metallicMap, uv).r;
    } else{
        metallic = material.metallic;
    }
    return metallic;
}

float getAmbientOcclusion() 
{
    float ambientOcclusion = 1.0;
    if(material.enableAmbientOcclusionMap){
        ambientOcclusion = texture(material.ambientOcclusionMap, uv).r;
    }
    return ambientOcclusion;
}

vec3 getAmbient(vec3 albedo, float ambientOcclusion) {
    vec3 ambient = vec3(scene.ambientStrength) * albedo * ambientOcclusion;
    return ambient;
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

    vec3 N = normalize(v_normals); // normal direction
    vec3 V = normalize(scene.cameraPosition - v_fragmentPosition); // view direction

    float dialectricReflecitivity = 0.04;
    vec3 F0 = mix(vec3(dialectricReflecitivity), albedo, metallic); // base reflectivity

    // get shadow
    float shadow = getShadow();

    // calculate each lights impact on object
    vec3 Lo = vec3(0.0);
    int iterations = shadow == 1.0 ? 0 : 1; // iterate over every light source, or none if object is in shadow
    for(int i = 0; i < iterations; ++i)
    {
        vec3 L = normalize(light.position - v_fragmentPosition); // light direction
        vec3 H = normalize(V + L); // halfway direction

        // cosine of angle between N and L, indicates effective light contribution from source
        float NdotL = max(dot(N, L), 0.0);
        if(NdotL == 0.0)
        {
            continue; // no light contribution from source, skip light
        }

        // per-light radiance
        // float distance = length(light.position - v_fragmentPosition);
        // float attenuation = 1.0 / (distance * distance);
        float attenuation = 1.0; // full attenuation for directional lights
        if(attenuation == 0.0) 
        {
            continue; // no light contribution from source, skip light
        }
        vec3 radiance = light.color * light.intensity * attenuation;
        
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
            
        // add to outgoing radiance          
        Lo += (diffuse + specular) * radiance * NdotL; 
    }
  
    // get ambient
    vec3 ambient = getAmbient(albedo, ambientOcclusion);

    // assemble shaded color
    vec3 color = ambient + Lo * (1.0 - shadow);
	
    // gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / gamma));

    // return shaded color
    return vec4(color, 1.0);
}

void main()
{
    uv = v_textureCoords * material.tiling + material.offset;
    FragColor = shadePBR();
}