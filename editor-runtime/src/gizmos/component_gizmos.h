#pragma once

#include "../core/ecs/ecs_collection.h"
#include "../core/rendering/gizmos/gizmos.h"

namespace ComponentGizmos
{

	// Renders the icons of various components such as lights and cameras
	void renderSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform);

	// Render box collider component
	void renderBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider);

	// Render sphere collider component
	void renderSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider);

};
