#pragma once

#include <ecs/ecs_collection.h>
#include <rendering/gizmos/gizmos.h>

namespace ComponentGizmos
{

	//
	// GENERAL
	//

	// Renders icon gizmos of all components in register
	void drawSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform);

	// Renders gizmos of all components for specific entity
	void drawEntityGizmos(IMGizmo& gizmos, EntityContainer& entity);

	//
	// COMPONENTS
	//

	void drawCamera(IMGizmo& gizmos, TransformComponent& transform, CameraComponent& camera);
	void drawBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider);
	void drawSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider);
	void drawPointLight(IMGizmo& gizmos, TransformComponent& transform, PointLightComponent& pointLight);
	void drawSpotlight(IMGizmo& gizmos, TransformComponent& transform, SpotlightComponent& spotlight);

};
