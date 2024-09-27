#pragma once

#include <string>
#include <glad/glad.h>
#include <sstream>
#include <vector>

#include "../engine/utils/log.h"
#include "../engine/utils/iohandler.h"
#include "../engine/utils/string_helper.h"

class Mesh
{
public:
	Mesh(std::string path);

	void bind();
	unsigned int getIndiceCount();
private:
	unsigned int id;
	unsigned int indice_count;

	std::vector<float> _vertices;
	std::vector<float> _texture_coords;
	std::vector<float> _normals;
	std::vector<int> _indices;

	void parseMesh(std::string path);
	std::vector<float> parseCoordinates(const std::string& line);
	void parseFace(const std::string& line);

	void generateVAO();
};