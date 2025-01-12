#include "component_gizmos.h"

#include <glm.hpp>

#include "../src/gizmos/editor_gizmo_color.h"

namespace ComponentGizmos {

	void renderSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform)
	{
		// Setup gizmos for icons
		gizmos.opacity = 0.08f;

		// Radius in which light gizmos get rendered
		const float radius = 50.0f;

		auto addIcon = [&gizmos, &cameraTransform, radius](std::string icon, glm::vec3 position) -> void {
			if (glm::distance(position, cameraTransform.position) > radius) return;
			gizmos.icon3d(IconPool::get(icon), position, cameraTransform, glm::vec3(0.8f));
		};

		// Render camera icons
		auto cameras = ECS::gRegistry.view<TransformComponent, CameraComponent>();
		for (auto [entity, transform, camera] : cameras.each()) {
			addIcon("camera_gizmo", transform.position);
		}

		// Render light icons (tmp)
		/*for (int i = 0; i < 1; i++) {
			addIcon("light_gizmo", glm::vec3(0.0f, 0.0f, 0.0f));
		}*/
	}

	void renderEntityGizmos(IMGizmo& gizmos, EntityContainer& entity)
	{
		// Render box collider
		if (entity.has<BoxColliderComponent>()) ComponentGizmos::renderBoxCollider(gizmos, entity.transform, entity.get<BoxColliderComponent>());

		// Render sphere collider
		if (entity.has<SphereColliderComponent>()) ComponentGizmos::renderSphereCollider(gizmos, entity.transform, entity.get<SphereColliderComponent>());
	}

	void renderBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::COLLIDER;
		gizmos.opacity = EditorGizmoColor::COLLIDER.a;
		gizmos.boxWire(transform.position, boxCollider.size, transform.rotation);
	}

	void renderSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::COLLIDER;
		gizmos.opacity = EditorGizmoColor::COLLIDER.a;
		gizmos.sphereWire(transform.position, sphereCollider.radius, transform.rotation);
	}

}