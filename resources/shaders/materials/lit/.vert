#version 330 core

layout (location = 0) in vec3 position_in;    // Input vertex position
layout (location = 1) in vec3 normals_in;     // Input normals
layout (location = 2) in vec2 textureCoords_in; // Input texture coordinates

uniform mat4 mvp; // Model-View-Projection matrix
uniform mat4 model; // Model-Projection matrix

out vec3 v_normals;          // Output variable for normals
out vec3 v_fragmentPosition;
out vec2 v_textureCoords;    // Output variable for texture coordinates

void main()
{
    gl_Position = mvp * vec4(position_in, 1.0);

    v_normals = mat3(transpose(inverse(model))) * normals_in;  // Pass normals in world space to fragment shader
    v_fragmentPosition = vec3(model * vec4(position_in, 1.0)); // Pass fragment position in screen space to fragment shader
    v_textureCoords = textureCoords_in;   // Pass texture coordinates to fragment shader
}