#include "component_gizmos.h"

#include <glm.hpp>

#include "../src/gizmos/editor_gizmo_color.h"
#include "../src/editor_systems/component_registry.h"

namespace ComponentGizmos {

	void drawSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform)
	{
		// Try draw scene icon of every component instance
		for (const auto& [name, component] : ComponentRegistry::get()) {
			component.tryDrawSceneIcons(gizmos, cameraTransform);
		}
	}

	void drawEntityGizmos(IMGizmo& gizmos, EntityContainer& entity)
	{
		// Try draw gizmo of every component type for entity
		for (const auto& [name, component] : ComponentRegistry::get()) {
			component.tryDrawGizmo(entity.root, gizmos);
		}
	}

	void drawCamera(IMGizmo& gizmos, TransformComponent& transform, CameraComponent& camera)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::CAMERA;
		gizmos.opacity = EditorGizmoColor::CAMERA.a;
		gizmos.boxWire(transform.position, glm::vec3(2.0f), transform.rotation);
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