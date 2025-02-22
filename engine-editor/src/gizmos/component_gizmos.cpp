#include "component_gizmos.h"

#include <glm.hpp>

#include "../src/gizmos/editor_gizmo_color.h"
#include "../src/editor_systems/component_registry.h"

namespace ComponentGizmos {

	void drawSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform)
	{
		// Setup gizmos for icons
		gizmos.opacity = 0.08f;

		// Render camera icons
		auto cameras = ECS::gRegistry.view<TransformComponent, CameraComponent>();
		for (auto [entity, transform, camera] : cameras.each()) {
			gizmos.icon3d(IconPool::get("camera_gizmo"), transform.position, cameraTransform, glm::vec3(0.8f));
		}
	}

	void drawEntityGizmos(IMGizmo& gizmos, EntityContainer& entity)
	{
		// Try draw gizmo of every component type for entity
		for (const auto& [name, component] : ComponentRegistry::get()) {
			component.tryDrawGizmo(entity.root, gizmos);
		}
	}

	void drawBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::COLLIDER;
		gizmos.opacity = EditorGizmoColor::COLLIDER.a;
		gizmos.boxWire(transform.position, boxCollider.size, transform.rotation);
	}

	void drawSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::COLLIDER;
		gizmos.opacity = EditorGizmoColor::COLLIDER.a;
		gizmos.sphereWire(transform.position, sphereCollider.radius, transform.rotation);
	}

}