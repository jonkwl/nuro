#include "bounding_volume.h"

BV_Sphere::BV_Sphere()
{
	center = glm::vec3(0.0f);
	radius = 0.0f;
}

bool BV_Sphere::intersectsFrustum(Frustum frustum)
{
	return true;
}
