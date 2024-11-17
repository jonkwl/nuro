#include "../src/rendering/model/model.h"

#pragma once
class MeshRenderer
{
public:
	MeshRenderer();

	Model* model; // Linked model, rendering target

	bool overwriteMaterials; // Check if mesh renderer overwrites models default materials
	std::vector<IMaterial*> materials; // Container for mesh renderers overwrite materials

	void forwardPass();
	void prePass();
	void shadowPass();

	static glm::mat4 currentModelMatrix;
	static glm::mat4 currentViewMatrix;
	static glm::mat4 currentProjectionMatrix;

	static glm::mat4 currentLightSpaceMatrix;
};