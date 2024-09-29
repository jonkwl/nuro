#version 330 core

layout (location = 0) in vec3 position_in;    // Input vertex position
layout (location = 1) in vec3 normals_in;     // Input normals
layout (location = 2) in vec2 textureCoords_in; // Input texture coordinates

uniform mat4 mvp; // Model-View-Projection matrix
uniform vec3 cameraPosition;

out vec3 normals;          // Output variable for normals
out vec2 textureCoords;    // Output variable for texture coordinates
out vec3 v_cameraPosition;

void main()
{
    gl_Position = mvp * vec4(position_in, 1.0);

    normals = normals_in;               // Pass normals to fragment shader
    textureCoords = textureCoords_in;   // Pass texture coordinates to fragment shader
    v_cameraPosition = cameraPosition;
}
