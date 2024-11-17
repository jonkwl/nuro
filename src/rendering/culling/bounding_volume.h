#pragma once

#include <glm.hpp>
#include "../src/camera/frustum.h"

class BoundingVolume
{
public:
	virtual bool intersectsFrustum(Frustum frustum) { return false; };
};

class BV_Sphere : public BoundingVolume {
public:
	BV_Sphere();

	glm::vec3 center;
	float radius;

	bool intersectsFrustum(Frustum frustum);
};
