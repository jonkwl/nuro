#version 330 core

out vec4 FragColor;         // Output color of the fragment

in vec3 normals;            // Input normal vector
in vec2 textureCoords;      // Input texture coordinates
in vec3 v_cameraPosition;      // Input texture coordinates

uniform vec4 baseColor; // Base color
uniform sampler2D baseTexture; // Texture sampler

void main()
{
    // set normals
    vec3 normalsProcessed = normalize(normals);

    // light variables
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightDirection = vec3(1.0, 0.0, 0.0);
    float lightStrength = 0.5;

    // ambient light
    float ambientStrength = 0.15;
    vec3 ambientColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambientColor * ambientStrength;

    // diffuse light
    float diffuseStrength = max(0.0, dot(lightDirection, normalsProcessed));
    vec3 diffuse = lightColor * diffuseStrength * lightStrength;

    // specular light
    vec3 viewSource = normalize(v_cameraPosition);
    vec3 reflectionSource = normalize(reflect(-lightDirection, normalsProcessed));
    float specularStrength = max(0.0, dot(viewSource, reflectionSource));
    float glossiness = 30.0;
    specularStrength = pow(specularStrength, glossiness);
    vec3 specular = lightColor * specularStrength * lightStrength;
    
    // lighting = ambient + diffuse + specular
    vec3 lighting = ambient + diffuse + specular;

    // final color = base color * lighting color
    vec4 finalColor = vec4(baseColor.x * lighting.x, baseColor.y * lighting.y, baseColor.z * lighting.z, 1.0);

    FragColor = texture(baseTexture, textureCoords) * finalColor;
}