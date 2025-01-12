#pragma once

#include "../core/ecs/ecs_collection.h"
#include "../core/rendering/gizmos/gizmos.h"

namespace ComponentGizmos
{

	// Renders icon gizmos of all components in register
	void renderSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform);

	// Renders gizmos of all components for specific entity
	void renderEntityGizmos(IMGizmo& gizmos, EntityContainer& entity);

	// Render box collider component gizmo
	void renderBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider);

	// Render sphere collider component gizmo
	void renderSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider);

};
