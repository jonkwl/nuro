#pragma once

#include <vector>
#include <glm.hpp>
#include "../engine/rendering/texture/texture.h"

struct VertexData {
    glm::vec3 Position;
    // glm::vec3 Normal;
    // glm::vec2 TexCoords;
};

class Mesh
{
public:
    Mesh(std::vector<VertexData> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    void bind();
    void render();
};