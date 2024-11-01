#version 330 core

out vec4 FragColor;

in vec3 v_normals;
in vec3 v_fragmentPosition;
in vec4 v_fragmentLightSpacePosition;
in vec2 v_textureCoords;

vec2 uv;
vec3 normalDirection;
vec3 lightDirection;

struct Scene {
    sampler2D shadowMap;
    vec3 cameraPosition;
    vec3 lightPosition;
};
uniform Scene scene;

struct Light {
    vec3 lightColor;
    float lightIntensity;
    vec3 ambientColor;
    float ambientStrength;
};
uniform Light light;

struct Material {
    vec4 baseColor;

    vec2 tiling;
    vec2 offset;

    bool enableDiffuseMap;
    sampler2D diffuseMap;

    bool enableNormalMap;
    sampler2D normalMap;

    bool enableSpecularMap;
    sampler2D specularMap;

    bool useLegacyLighting;
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
    vec3 lightDirection = normalize(scene.lightPosition - v_fragmentPosition);

    float maxBias = 0.01;
    float minBias = 0.005;
    float bias = max(maxBias * dot(normalDirection, lightDirection), minBias);
    bias = 0.001f;

    float shadow = pcf(projectionCoordinates, currentDepth, bias, 1.0, 3);

    return shadow;
}

vec3 getAmbient()
{
    vec3 ambient = light.ambientColor * light.ambientStrength;
    return ambient;
}

vec3 getDiffuse()
{
    float diffuseValue = max(0.0, dot(lightDirection, normalDirection));
    vec3 diffuse = light.lightColor * diffuseValue * light.lightIntensity;
    return diffuse;
}

vec3 getSpecular()
{
    float specularStrength = 0.75;
    float glossiness = 32.0;
    vec3 viewDirection = normalize(scene.cameraPosition - v_fragmentPosition);
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float spec = pow(max(dot(halfwayDirection, normalDirection), 0.0), glossiness);
    vec3 specular = light.lightColor * spec * specularStrength * light.lightIntensity;
    return specular;
}

vec3 getLegacyLighting() {
    // ambient light
    vec3 ambient = getAmbient();

    // diffuse light
    vec3 diffuse = getDiffuse();

    // specular light
    vec3 specular = getSpecular();

    // shadow
    float shadow = getShadow();

    // final lighting value
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    return lighting;
}

vec3 getPBRLighting() {
    return vec3(0.0);
}

void main()
{
    // set uv
    uv = v_textureCoords * material.tiling + material.offset;

    // set normals
    normalDirection = normalize(v_normals);

    // light variables
    lightDirection = normalize(scene.lightPosition - v_fragmentPosition);

    // calculate lighting

    vec3 lighting = vec3(0.0);
    if(!material.useLegacyLighting){
        lighting = getPBRLighting(); // pbr lighting
    } else {
        lighting = getLegacyLighting(); // simpler legacy lighting
    }

    // calculate and set final color

    vec4 finalColor = vec4(vec3(material.baseColor) * lighting, 1.0);
    if(material.enableDiffuseMap){
        finalColor = texture(material.diffuseMap, uv) * finalColor;
    }

    FragColor = finalColor;
}