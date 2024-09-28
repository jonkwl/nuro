#pragma once

#include <vector>
#include <glm.hpp>
#include "../engine/rendering/texture/texture.h"

struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

class Mesh
{
public:
    Mesh(std::vector<VertexData> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, int materialIndex);

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures;

    void bind();
    void render();

    unsigned int getMaterialIndex();
private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    unsigned int materialIndex;
};