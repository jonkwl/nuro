#pragma once

#include <vector>
#include <glm.hpp>

struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh
{
public:
    Mesh(std::vector<VertexData> vertices, std::vector<unsigned int> indices, int materialIndex);

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;

    void bind();

    unsigned int getMaterialIndex();
private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    unsigned int materialIndex;
};