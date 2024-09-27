#include "mesh.h"

Mesh::Mesh(std::string path)
{
	parseMesh(path);
    generateVAO();
}

void Mesh::bind() {
    glBindVertexArray(id);
}

unsigned int Mesh::getIndiceCount() {
    return indice_count;
}

void Mesh::parseMesh(std::string path)
{
	std::vector<std::string> source = IOHandler::ReadFileLines(path);

	for (int i = 0; i < source.size(); i++) {
		std::string line = source[i];

		if (StringHelper::startsWith(line, "vt")) {

			std::vector<float> append = parseCoordinates(line);
			_texture_coords.insert(_texture_coords.end(), append.begin(), append.end());

		}
		else if (StringHelper::startsWith(line, "vn")) {

			std::vector<float> append = parseCoordinates(line);
			_normals.insert(_normals.end(), append.begin(), append.end());

		}
		else if (StringHelper::startsWith(line, "v")) {

			std::vector<float> append = parseCoordinates(line);
			_vertices.insert(_vertices.end(), append.begin(), append.end());

		}
		else if (StringHelper::startsWith(line, "f")) {
			parseFace(line);
		}
	}
}

std::vector<float> Mesh::parseCoordinates(const std::string& line) {
	std::vector<float> coords;
	std::vector<std::string> elements = StringHelper::split(line, " ");
	for (int i = 0; i < elements.size(); i++) {
		std::stringstream ss(elements[i]);
		float coord;
		ss >> coord;
		if (!ss.fail()) {
			coords.push_back(coord);
		}
	}
	return coords;
}

void Mesh::parseFace(const std::string& line) {
	std::vector<std::string> vertex = StringHelper::split(line, " ");
	for (int a = 0; a < vertex.size(); a++) {
		std::vector<std::string> vertex_data = StringHelper::split(vertex[a], "/");
		for (int b = 0; b < vertex_data.size(); b++) {
			// Parse first element of vertex data: Vertex coordinate (resulting in indice index)
			if (b == 0) {
				std::stringstream ss(vertex_data[b]);
				int indice;
				ss >> indice;
				if (!ss.fail()) {
					_indices.push_back(indice);
				}
			}
		}
	}
}

void Mesh::generateVAO()
{
    // Define vertices and indices
    unsigned int vertice_components = 3; // Only position now

	/*float* vertices = new float[100];
	for (int i = 0; i < _vertices.size(); i++) {
		vertices[i] = _vertices[i];
	}

    unsigned int* indices = new unsigned int[_indices.size()];
	for (int i = 0; i < _indices.size(); i++) {
		indices[i] = _indices[i];
	}

	indice_count = _indices.size();*/

	std::vector<float> vertices = {
		1.000000, -1.000000, -1.000000,
		1.000000, -1.000000, 1.000000,
		-1.000000, -1.000000, 1.000000,
		-1.000000, -1.000000, -1.000000,
		1.000000, 1.000000, -1.000000,
		0.999999, 1.000000, 1.000001,
		-1.000000, 1.000000, 1.000000
		-1.000000, 1.000000, - 1.000000
	};

	int indices[] = { 
		12, 13, 14,
		0, 1, 2,
		9, 10, 11
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

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