#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <string>
#include <sstream>
#include <vector>

#include "../engine/utils/log.h"
#include "../engine/utils/iohandler.h"
#include "../engine/utils/string_helper.h"

struct Face {
	glm::ivec3 vertice_coords_index;
};

class Mesh
{
public:
	Mesh(std::string path);

	void bind();
	unsigned int getIndiceCount();
private:
	unsigned int id;
	unsigned int indice_count;

	std::vector<glm::vec3> vertice_coords; // All vertices as vector3 coordinates
	std::vector<glm::vec3> texture_coords; // All textures as vector3 coordinates (z will usually not be set)
	std::vector<glm::vec3> normal_coords; // All normals as vector3 coordinates
	std::vector<Face> faces; // Each face

	void parseMesh(std::string path);
	glm::vec3 parseCoordinates(const std::string& line);
	void parseFace(const std::string& line);

	void generateVAO();

	glm::ivec3 resolve_face_vertice(int vertice);
};