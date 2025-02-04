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
			// Optimization: Refetching icons from icon pool each render iteration isn't needed, can be cached
			gizmos.icon3d(IconPool::get(icon), position, cameraTransform, glm::vec3(0.8f));
		};

		// Render camera icons
		auto cameras = ECS::gRegistry.view<TransformComponent, CameraComponent>();
		for (auto [entity, transform, camera] : cameras.each()) {
			addIcon("camera", transform.position);
		}

		// Render directional light icons
		auto directionalLights = ECS::gRegistry.view<TransformComponent, DirectionalLightComponent>();
		for (auto [entity, transform, directionalLight] : directionalLights.each()) {
			addIcon("light", transform.position);
		}

		// Render point light icons
		auto pointLights = ECS::gRegistry.view<TransformComponent, PointLightComponent>();
		for (auto [entity, transform, pointLight] : pointLights.each()) {
			addIcon("light", transform.position);
		}

		// Render spotlight icons
		auto spotlights = ECS::gRegistry.view<TransformComponent, SpotlightComponent>();
		for (auto [entity, transform, spotlight] : spotlights.each()) {
			addIcon("light", transform.position);
		}
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