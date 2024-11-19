#include "../src/rendering/model/model.h"
#include "../src/rendering/culling/bounding_volume.h"

class Entity;

#pragma once
class MeshRenderer
{
public:
	MeshRenderer(Entity* parentEntity);

	Model* model; // Linked model, rendering target

	BoundingVolume* volume; // Bounding volume for the mesh renderer

	std::vector<IMaterial*> materials; // Container for mesh renderers materials

	void forwardPass();
	void prePass();
	void shadowPass();

	static glm::mat4 currentViewMatrix; // Static view matrix cache for current frame
	static glm::mat4 currentProjectionMatrix; // Static projection matrix cache for current frame

	static glm::mat4 currentLightSpaceMatrix; // Static light space matrix cache for current frame
private:
	glm::mat4 currentModelMatrix; // Model matrix cache for current frame
	glm::mat4 currentMvpMatrix; // MVP matrix cache for current frame

	Entity* parentEntity; // Linked parent entity

	void performFrustumCulling();
	bool intersectsFrustum;

	bool isCulled();
};