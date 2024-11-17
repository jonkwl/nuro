#include "bounding_volume.h"

#include "../src/rendering/gizmos/quick_gizmo.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/model/model.h"

BoundingSphere::BoundingSphere()
{
	center = glm::vec3(0.0f);
	radius = 0.0f;
}

void BoundingSphere::update(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    // Get models metrics
    ModelMetrics metrics = model->getMetrics();

    // Calculate center position: 
    // Position of models origin offset by the world position
	center = metrics.origin + position;

    // Calculate radius:
    // Maximum distance of a models vertice to models origin divided by 2 (because its a radius, not diameter) times biggest world scale factor
    radius = (metrics.furthest / 2) * std::max({ scale.x, scale.y, scale.z });

	QuickGizmo::color = GizmoColor::RED;
	QuickGizmo::sphereWire(center, radius);
}

bool BoundingSphere::intersectsFrustum(Frustum frustum)
{
    return true; // tmp
    for (int i = 0; i < 6; ++i) {
        const glm::vec4& plane = frustum.planes[i];
        float distance = glm::dot(glm::vec3(plane), center) + plane.w;
        if (distance > radius) {
            return false;
        }
    }
    return true;
}
