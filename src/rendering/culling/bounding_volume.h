#pragma once

#include <glm.hpp>

#include "../src/camera/camera.h"

class Model;

class BoundingVolume
{
public:
	virtual void update(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {};
	virtual bool intersectsFrustum(Frustum frustum) { return false; };
	virtual float getDistance(glm::vec3 point) { return 0.0f; }
};

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere();
	void update(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	bool intersectsFrustum(Frustum frustum);
	float getDistance(glm::vec3 point);

	glm::vec3 center;
	float radius;
};

class BoundingAABB : public BoundingVolume {
public:
	BoundingAABB();
	void update(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	bool intersectsFrustum(Frustum frustum);
	float getDistance(glm::vec3 point);

	glm::vec3 min;
	glm::vec3 max;
};
