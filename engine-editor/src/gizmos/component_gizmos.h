#pragma once

#include "../src/core/ecs/ecs_collection.h"
#include "../src/core/rendering/gizmos/gizmos.h"

namespace ComponentGizmos
{

	// Renders icon gizmos of all components in register
	void drawSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform);

	// Renders gizmos of all components for specific entity
	void drawEntityGizmos(IMGizmo& gizmos, EntityContainer& entity);

	// Draws a camera component gizmo
	void drawCamera(IMGizmo& gizmos, TransformComponent& transform, CameraComponent& camera);

	// Draws a box collider component gizmo
	void drawBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider);

	// Draws a sphere collider component gizmo
	void drawSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider);

};
