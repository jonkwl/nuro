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

#include "../engine/rendering/model/mesh.h"
#include "../engine/rendering/material/imaterial.h"

struct Face {
	glm::ivec3 vertice_coords_index;
};

class Model
{
public:
	Model(std::string path, IMaterial* material);
	std::vector<Mesh*> meshes;

	IMaterial* material;

	void render(glm::mat4 mvp);
private:
	std::string directory;

	void resolveModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
};