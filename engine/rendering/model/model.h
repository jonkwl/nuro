#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <string>
#include <sstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../engine/utils/log.h"
#include "../engine/utils/iohandler.h"
#include "../engine/utils/string_helper.h"

struct Face {
	glm::ivec3 vertice_coords_index;
};

class Model
{
public:
	Model(std::string path);

	void bind();
	unsigned int getIndiceCount();
private:
	unsigned int id;
	unsigned int indice_count;

	std::vector<aiMesh*> meshes;
	std::string directory;

	void resolveModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);
	aiMesh* processMesh(aiMesh* mesh, const aiScene* scene);

	void generateVAO();
};