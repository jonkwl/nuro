#version 330 core

out vec4 FragColor;

in vec3 v_normals;
in vec3 v_fragmentPosition;
in vec4 v_fragmentLightSpacePosition;
in vec2 v_textureCoords;

uniform vec4 baseColor;

uniform sampler2D shadowMap;

uniform bool enableDiffuseMap;
uniform sampler2D diffuseMap;

uniform bool enableSpecularMap;
uniform sampler2D specularMap;

uniform bool enableNormalMap;
uniform sampler2D normalMap;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float lightIntensity;

float shadowPCF(sampler2D shadowMap, vec3 projectionCoords, float currentDepth, float bias, float smoothing, int kernelRadius) {
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    int sampleCount = 0;
    float weightSum = 0.0;

    for (int x = -kernelRadius; x <= kernelRadius; ++x) {
        for (int y = -kernelRadius; y <= kernelRadius; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize * smoothing;
            float pcfDepth = texture(shadowMap, projectionCoords.xy + offset).r;
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

    float closestDepth = texture(shadowMap, projectionCoordinates.xy).r;
    float currentDepth = projectionCoordinates.z;

    vec3 normalDirection = normalize(v_normals);
    vec3 lightDirection = normalize(lightPosition - v_fragmentPosition);

    float maxBias = 0.01;
    float minBias = 0.005;
    float bias = max(maxBias * dot(normalDirection, lightDirection), minBias);
    bias = 0.001f;

    float shadow = shadowPCF(shadowMap, projectionCoordinates, currentDepth, bias, 1.0, 3);

    return shadow;
}

void main()
{
    // set normals
    vec3 normalDirection = normalize(v_normals);

    // light variables
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightDirection = normalize(lightPosition - v_fragmentPosition);
    // float lightIntensity = 0.3;

    // ambient light
    float ambientStrength = 0.025; 
    vec3 ambientColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambientColor * ambientStrength;

    // diffuse light
    float diffuseValue = max(0.0, dot(lightDirection, normalDirection));
    vec3 diffuse = lightColor * diffuseValue * lightIntensity;

    // specular light
    float specularStrength = 0.75;
    float glossiness = 32.0;
    vec3 viewDirection = normalize(cameraPosition - v_fragmentPosition);
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float spec = pow(max(dot(halfwayDirection, normalDirection), 0.0), glossiness);
    vec3 specular = lightColor * spec * specularStrength * lightIntensity;

    // shadow
    float shadow = getShadow();

    // final lighting value
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    // final color = base color * lighting color
    vec4 finalColor = vec4(baseColor.x * lighting.x, baseColor.y * lighting.y, baseColor.z * lighting.z, 1.0);

    if(enableDiffuseMap){
        finalColor = texture(diffuseMap, v_textureCoords) * finalColor;
    }

    FragColor = finalColor;
}