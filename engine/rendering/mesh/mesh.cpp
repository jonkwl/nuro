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
			texture_coords.push_back(parseCoordinates(line));
		}
		else if (StringHelper::startsWith(line, "vn")) {
			normal_coords.push_back(parseCoordinates(line));
		}
		else if (StringHelper::startsWith(line, "v")) {
			vertice_coords.push_back(parseCoordinates(line));
		}
		else if (StringHelper::startsWith(line, "f")) {
			parseFace(line);
		}
	}

	// For debugging vertice coords & face vertice indexes
	/*for (int i = 0; i < vertice_coords.size(); i++) {
		glm::vec3 coords = vertice_coords[i];
		Log::printProcessInfo("Vertice " + std::to_string(i + 1) + ": x" + std::to_string(coords.x) + ": y" + std::to_string(coords.y) + ": z" + std::to_string(coords.z));
	}

	for (int i = 0; i < faces.size(); i++) {
		Face face = faces[i];
		Log::printProcessInfo("Vertice Indexes of Face " + std::to_string(i + 1) + ": " + std::to_string(face.vertice_coords_index.x) + " " + std::to_string(face.vertice_coords_index.y) + " " + std::to_string(face.vertice_coords_index.z));
	}*/
}

glm::vec3 Mesh::parseCoordinates(const std::string& line) {
	glm::vec3 coords(0.0f);
	std::vector<std::string> elements = StringHelper::split(line, " ");
	int foundCoords = 0;
	for (int i = 0; i < elements.size(); i++) {
		std::stringstream ss(elements[i]);
		float coord;
		ss >> coord;
		if (!ss.fail()) {
			foundCoords++;
			switch (foundCoords) {
			case 1:
				coords.x = coord;
				break;
			case 2:
				coords.y = coord;
				break;
			case 3:
				coords.z = coord;
				break;
			}
		}
	}
	return coords;
}

// PROOF OF CONCEPT TESTING CODE!
void Mesh::parseFace(const std::string& line) {
	std::vector<std::string> vertex = StringHelper::split(line, " ");
	Face face;

	int vertice_coords_found = 0;

	for (int a = 0; a < vertex.size(); a++) {
		std::vector<std::string> vertex_data = StringHelper::split(vertex[a], "/");
		for (int b = 0; b < vertex_data.size(); b++) {
			// Parse first element of vertex data: Vertex coordinate (resulting in indice index)
			if (b == 0) {
				std::stringstream ss(vertex_data[b]);
				int vertex_index;
				ss >> vertex_index;
				if (!ss.fail()) {
					vertice_coords_found++;
					vertex_index--;
					switch (vertice_coords_found) {
					case 1:
						face.vertice_coords_index.x = vertex_index;
						break;
					case 2:
						face.vertice_coords_index.y = vertex_index;
						break;
					case 3:
						face.vertice_coords_index.z = vertex_index;
						break;
					}
				}
			}
		}
	}

	faces.push_back(face);
}

void Mesh::generateVAO()
{
    // Define vertices and indices
    unsigned int vertice_components = 3; // Only position now

	std::vector<float> vertices;
	for (int i = 0; i < vertice_coords.size(); i++) {
		glm::vec3 vertice = vertice_coords[i];
		vertices.insert(vertices.end(), { vertice.x, vertice.y, vertice.z });
	}

	std::vector<int> indices{
		1, 2, 3
	};
	indice_count = indices.size();

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // Unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    id = vao;
}

glm::ivec3 Mesh::resolve_face_vertice(int vertice)
{
	return glm::vec3(vertice * 3, vertice * 3 + 1, vertice * 3 + 2);
}