#include "../src/rendering/model/model.h"

#pragma once
class MeshRenderer
{
public:
	MeshRenderer(Model* model);

	Model* model; // Linked model, rendering target

	bool overwriteMaterials; // Check if mesh renderer overwrites models default materials
	std::vector<IMaterial*> materials; // Container for mesh renderers overwrite materials
};