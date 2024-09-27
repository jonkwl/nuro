#include "cube_model.h"

CubeModel::CubeModel()
{
    // Define vertices and indices
    unsigned int vertice_components = 3; // Only position now

    float vertices[] = {
        // Position (x, y, z)

        // Front face
        0.5f,  0.5f,  0.5f,      // Vertex 0
       -0.5f,  0.5f,  0.5f,      // Vertex 1
        0.5f, -0.5f,  0.5f,      // Vertex 2
       -0.5f, -0.5f,  0.5f,      // Vertex 3

       // Back face
        0.5f,  0.5f, -0.5f,      // Vertex 4
       -0.5f,  0.5f, -0.5f,      // Vertex 5
        0.5f, -0.5f, -0.5f,      // Vertex 6
       -0.5f, -0.5f, -0.5f,      // Vertex 7

       // Left face
       -0.5f,  0.5f,  0.5f,      // Vertex 8 (same as Vertex 1)
       -0.5f,  0.5f, -0.5f,      // Vertex 9 (same as Vertex 5)
       -0.5f, -0.5f,  0.5f,      // Vertex 10 (same as Vertex 3)
       -0.5f, -0.5f, -0.5f,      // Vertex 11 (same as Vertex 7)

       // Right face
        0.5f,  0.5f,  0.5f,      // Vertex 12 (same as Vertex 0)
        0.5f,  0.5f, -0.5f,      // Vertex 13 (same as Vertex 4)
        0.5f, -0.5f,  0.5f,      // Vertex 14 (same as Vertex 2)
        0.5f, -0.5f, -0.5f,      // Vertex 15 (same as Vertex 6)

        // Top face
         0.5f,  0.5f,  0.5f,      // Vertex 16 (same as Vertex 0)
        -0.5f,  0.5f,  0.5f,      // Vertex 17 (same as Vertex 1)
         0.5f,  0.5f, -0.5f,      // Vertex 18 (same as Vertex 4)
        -0.5f,  0.5f, -0.5f,      // Vertex 19 (same as Vertex 5)

        // Bottom face
         0.5f, -0.5f,  0.5f,      // Vertex 20 (same as Vertex 2)
        -0.5f, -0.5f,  0.5f,      // Vertex 21 (same as Vertex 3)
         0.5f, -0.5f, -0.5f,      // Vertex 22 (same as Vertex 6)
        -0.5f, -0.5f, -0.5f       // Vertex 23 (same as Vertex 7)
    };

    unsigned int indices[] = {
        // Front face
        0, 1, 2,    // Triangle 1
        1, 2, 3,    // Triangle 2

        // Back face
        4, 5, 6,    // Triangle 3
        5, 6, 7,    // Triangle 4

        // Left face
        8, 9, 10,   // Triangle 5
        9, 10, 11,  // Triangle 6

        // Right face
        12, 13, 14, // Triangle 7
        13, 14, 15, // Triangle 8

        // Top face
        16, 17, 18, // Triangle 9
        17, 18, 19, // Triangle 10

        // Bottom face
        20, 21, 22, // Triangle 11
        21, 22, 23  // Triangle 12
    };
    indice_count = sizeof(indices) / sizeof(indices[0]);

    // Create vertex array object (VAO)
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer object (VBO) and set data
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attrib pointers of VAO using data from VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create element buffer object (EBO)
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    id = vao;
}

void CubeModel::bind()
{
    glBindVertexArray(id);
}

unsigned int CubeModel::getIndiceCount() {
    return indice_count;
}
