#pragma once

#include <glm.hpp>

#include "../src/camera/camera.h"

class BoundingVolume
{
public:
	virtual void update(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {};
	virtual bool intersectsFrustum(Frustum frustum) { return false; };
};

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere();
	void update(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	bool intersectsFrustum(Frustum frustum);

	glm::vec3 center;
	float radius;
};
