#pragma once

#include <glm.hpp>

#include "../src/camera/camera.h"

class Model;

class BoundingVolume
{
public:
	virtual void update(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {};
	virtual bool intersectsFrustum(Frustum frustum) { return false; };
};

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere();
	void update(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	bool intersectsFrustum(Frustum frustum);

	glm::vec3 center;
	float radius;
};
