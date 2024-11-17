#include "bounding_volume.h"

#include "../src/rendering/gizmos/quick_gizmo.h"

BoundingSphere::BoundingSphere()
{
	center = glm::vec3(0.0f);
	radius = 0.0f;
}

void BoundingSphere::update(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	center = position;
	float maxScale = std::fmaxf(scale.x, std::fmaxf(scale.y, scale.z));
	radius = maxScale;

	QuickGizmo::color = GizmoColor::RED;
	QuickGizmo::sphereWire(center, radius);
}

bool BoundingSphere::intersectsFrustum(Frustum frustum)
{
	return true;
}
