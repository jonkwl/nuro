#include "mesh.h"

#include <glad/glad.h>

#include "../src/rendering/texture/texture.h"

Mesh::Mesh(std::vector<VertexData> vertices, std::vector<unsigned int> indices, int materialIndex) : vertices(vertices),
                                                                                                     indices(indices),
                                                                                                     vao(0),
                                                                                                     vbo(0),
                                                                                                     ebo(0),
                                                                                                     materialIndex(materialIndex)
{
    // Generate VAO, VBO and EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Bind VAO
    glBindVertexArray(vao);

    // Bind VBO, allocate its memory send vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

    // Bind EBO, allocate its memory and send indice data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set attributes for VAO
    // Vertex position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, position));
    // Normal attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, normal));
    // Texture coordinates attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, uv));
    // Tangent attribute (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, tangent));
    // Bitangent attribute (location = 3)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, bitangent));

    // Unbind VAO, ABO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::bind()
{
    glBindVertexArray(vao);
}

unsigned int Mesh::getMaterialIndex()
{
    return materialIndex;
}