#include "bounding_volume.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "../src/rendering/gizmos/quick_gizmo.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/model/model.h"

BoundingSphere::BoundingSphere()
{
    center = glm::vec3(0.0f);
    radius = 0.0f;
}

void BoundingSphere::update(Model *model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    // Get models metrics
    ModelMetrics metrics = model->getMetrics();

    // Calculate center position:
    // Position of models origin offset by the world position
    center = metrics.origin + position;

    // Calculate radius:
    // Maximum distance of a models vertice to models origin divided by 2 (because its a radius, not diameter) times biggest world scale factor
    radius = (metrics.furthest * 0.5f) * std::max({scale.x, scale.y, scale.z});

    // Draw bounding volume
    QuickGizmo::color = GizmoColor::RED;
    // QuickGizmo::sphereWire(center, radius);
}

bool BoundingSphere::intersectsFrustum(Frustum frustum)
{
    return true;
}

float BoundingSphere::getDistance(glm::vec3 point)
{
    // Return smallest distance of given point to spheres surface
    float signedDistance = glm::length(point - center) - radius;
    return std::fmaxf(signedDistance, 0.0f);
}

BoundingAABB::BoundingAABB()
{
    min = glm::vec3(0.0f);
    max = glm::vec3(0.0f);
}

void BoundingAABB::update(Model *model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    // Get model's metrics
    ModelMetrics metrics = model->getMetrics();

    // Calculate half extents of the model
    glm::vec3 modelHalfSize = (metrics.maxPoint - metrics.minPoint) * 0.5f;
    glm::vec3 worldHalfSize = scale * 0.5f;
    glm::vec3 absoluteHalfSize = modelHalfSize * worldHalfSize;

    // Prepare rotation
    glm::quat quaternion = glm::quat(glm::radians(rotation));

    // Compute rotated half extents along each axis
    glm::vec3 rotatedX = glm::vec3(quaternion * glm::vec4(absoluteHalfSize.x, 0.0f, 0.0f, 0.0f));
    glm::vec3 rotatedY = glm::vec3(quaternion * glm::vec4(0.0f, absoluteHalfSize.y, 0.0f, 0.0f));
    glm::vec3 rotatedZ = glm::vec3(quaternion * glm::vec4(0.0f, 0.0f, absoluteHalfSize.z, 0.0f));

    // Calculate poisiton offset
    glm::vec3 positionOffset = position + metrics.origin;

    // Calculate the AABB in world space using rotated half extents
    glm::vec3 _min = positionOffset - glm::abs(rotatedX) - glm::abs(rotatedY) - glm::abs(rotatedZ);
    glm::vec3 _max = positionOffset + glm::abs(rotatedX) + glm::abs(rotatedY) + glm::abs(rotatedZ);

    // Set the final bounding box
    min = _min;
    max = _max;

    // Draw bounding volume
    glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 size = max - min;
    QuickGizmo::color = GizmoColor::DARK_RED;
    // QuickGizmo::boxWire(center, size);
}

bool BoundingAABB::intersectsFrustum(Frustum frustum)
{
    return true;
}

float BoundingAABB::getDistance(glm::vec3 point)
{
    // Return smallest distance of given point to AABB's surface
    glm::vec3 clampedPoint = glm::clamp(point, min, max);
    glm::vec3 difference = point - clampedPoint;
    return glm::length(difference);
}