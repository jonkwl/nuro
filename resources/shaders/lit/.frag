#version 330 core

out vec4 FragColor;         // Output color of the fragment

in vec3 v_normals;            // Input normal vector
in vec2 v_textureCoords;      // Input texture coordinates
in vec3 v_fragmentPosition;

uniform vec4 baseColor; // Base color
uniform sampler2D baseTexture; // Texture sampler
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

void main()
{
    // set normals
    vec3 normals = normalize(v_normals);

    // light variables
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightDirection = normalize(lightPosition - v_fragmentPosition);
    float lightStrength = 0.5;

    // ambient light
    float ambientStrength = 0.15;
    vec3 ambientColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambientColor * ambientStrength;

    // diffuse light
    float diffuseValue = max(0.0, dot(lightDirection, normals));
    vec3 diffuse = lightColor * diffuseValue * lightStrength;

    // specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPosition - v_fragmentPosition);
    vec3 reflectDir = reflect(-lightDirection, normals);
    float glossiness = 32.0;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = lightColor * spec * specularStrength * lightStrength;
    
    // lighting = ambient + diffuse + specular
    vec3 lighting = ambient + diffuse + specular;

    // final color = base color * lighting color
    vec4 finalColor = vec4(baseColor.x * lighting.x, baseColor.y * lighting.y, baseColor.z * lighting.z, 1.0);

    FragColor = texture(baseTexture, v_textureCoords) * finalColor;
}