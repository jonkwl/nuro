#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <string>
#include <sstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../src/utils/log.h"
#include "../src/utils/iohandler.h"
#include "../src/utils/string_helper.h"
#include "../src/rendering/model/mesh.h"
#include "../src/rendering/material/imaterial.h"

class Model
{
public:
	Model(std::string path);
	Model(std::string path, IMaterial* defaultMaterial);
	Model(std::string path, std::vector<IMaterial*> defaultMaterials);
	std::vector<Mesh*> meshes;

	std::vector<IMaterial*> defaultMaterials;

	bool castsShadow = true;
private:
	std::string directory;

	void resolveModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<aiMaterial*> model_materials;
};