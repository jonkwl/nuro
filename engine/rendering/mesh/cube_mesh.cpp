#include "cube_mesh.h"

unsigned int CubeMesh::generate(unsigned int& indice_count)
{
	// define vertices and indices
	unsigned int vertice_components = 9;
	/*float vertices[] = {
		// position (vec3)      // base_color (vec4)       // texture coords
		0.5f,  -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
		-0.5f, -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
		0.5f,   0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3,
	};*/
	float vertices[] = {
		// Position (x, y, z)     // Color (r, g, b, a)   // Texture coords (u, v)

		// Front face
		0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 0
	   -0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 1
		0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 2
	   -0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 3

	   // Back face
	   0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 4
	  -0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 5
	   0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 6
	  -0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 7

	  // Left face
	 -0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 8 (same as Vertex 1)
	 -0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 9 (same as Vertex 5)
	 -0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 10 (same as Vertex 3)
	 -0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 11 (same as Vertex 7)

	 // Right face
	 0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 12 (same as Vertex 0)
	 0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 13 (same as Vertex 4)
	 0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 14 (same as Vertex 2)
	 0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 15 (same as Vertex 6)

	 // Top face
	 0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 16 (same as Vertex 0)
	-0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 17 (same as Vertex 1)
	 0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 18 (same as Vertex 4)
	-0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 19 (same as Vertex 5)

	// Bottom face
	0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 20 (same as Vertex 2)
   -0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 21 (same as Vertex 3)
	0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 22 (same as Vertex 6)
   -0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f  // Vertex 23 (same as Vertex 7)
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

	// create vertex array object (vao)
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create vertex buffer object (vbo) and set data
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set vertex attrib pointers of vao using data from vbo
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// create element buffer object (ebo)
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// unbind vao, vbo and ebo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return vao;
}
