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

	static glm::mat4 currentModelMatrix;
	static glm::mat4 currentViewMatrix;
	static glm::mat4 currentProjectionMatrix;

	static glm::mat4 currentLightSpaceMatrix;
private:
	Entity* parentEntity; // Linked parent entity

	void performFrustumCulling();
	bool intersectsFrustum;

	bool isCulled();
};