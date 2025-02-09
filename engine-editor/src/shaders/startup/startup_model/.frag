#version 330 core

// input attributes from vertex shader
in vec3 v_normal; // normal vector at each fragment
in vec2 v_uv;     // texture coordinates

// uniforms for material properties and texture
uniform sampler2D colorTexture;  // color texture for the object
uniform vec4 baseColor;          // base color of the object

// shading constants
const vec3 lightPosition = vec3(3.0, 3.0, 5.5);      // position of light source
const vec3 lightColor = vec3(1.0);         // color of light source
const vec3 cameraPosition = vec3(0.0);     // camera position

const float ambientStrength = 0.1;   // ambient light strength
const float diffuseStrength = 0.8;   // diffuse light strength
const float specularStrength = 1.0;  // specular strength
const float shininess = 32.0;        // shininess coefficient

// outputs the fragment color
out vec4 FragColor;

// main shading function implementing blinn-phong model
vec4 shade() {
    // normalize normal vector (ensure correct lighting calculations)
    vec3 normal = normalize(v_normal);
    
    // calculate direction to light source
    vec3 lightDir = normalize(lightPosition - vec3(0.0, 0.0, 0.0)); // camera at (0,0,0)
    
    // ambient lighting component (slight constant lighting)
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse lighting component (lambertian reflection)
    float diff = max(dot(normal, lightDir), 0.0); // diffuse factor
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    // specular lighting component (blinn-phong specular reflection)
    // calculate view direction (from fragment to camera)
    vec3 viewDir = normalize(cameraPosition - vec3(0.0, 0.0, 0.0)); // camera at (0,0,0)
    
    // halfway vector between light direction and view direction
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // calculate specular highlight (using blinn-phong model)
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // combine all lighting components
    vec3 lighting = ambient + diffuse + specular;
    
    // sample color from texture and apply lighting
    vec4 colorSample = texture(colorTexture, v_uv).rgba;
    
    // final color = material color * lighting
    return baseColor * colorSample * vec4(lighting, 1.0);
}

void main() {
    // output final fragment color using shade function
    FragColor = shade();
}