#include "component_gizmos.h"

#include <glm/glm.hpp>

#include <transform/transform.h>

#include "../gizmos/editor_gizmo_color.h"
#include "../reflection/component_registry.h"

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
			component.tryDrawGizmo(entity.handle(), gizmos);
		}
	}

	void drawCamera(IMGizmo& gizmos, TransformComponent& transform, CameraComponent& camera)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::CAMERA;
		gizmos.opacity = EditorGizmoColor::CAMERA.a;
		gizmos.boxWire(Transform::getPosition(transform, Space::WORLD), glm::vec3(2.0f), Transform::getRotation(transform, Space::WORLD));
	}

	void drawBoxCollider(IMGizmo& gizmos, TransformComponent& transform, BoxColliderComponent& boxCollider)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::COLLIDER;
		gizmos.opacity = EditorGizmoColor::COLLIDER.a;
		gizmos.boxWire(Transform::getPosition(transform, Space::WORLD), boxCollider.size, Transform::getRotation(transform, Space::WORLD));
	}

	void drawSphereCollider(IMGizmo& gizmos, TransformComponent& transform, SphereColliderComponent& sphereCollider)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::COLLIDER;
		gizmos.opacity = EditorGizmoColor::COLLIDER.a;
		gizmos.sphereWire(Transform::getPosition(transform, Space::WORLD), sphereCollider.radius, Transform::getRotation(transform, Space::WORLD));
	}

	void drawPointLight(IMGizmo& gizmos, TransformComponent& transform, PointLightComponent& pointLight)
	{
		gizmos.foreground = false;
		gizmos.color = pointLight.color;
		gizmos.opacity = 0.1f;
		gizmos.sphereWire(Transform::getPosition(transform, Space::WORLD), pointLight.range, glm::identity<glm::quat>());
	}

	void drawSpotlight(IMGizmo& gizmos, TransformComponent& transform, SpotlightComponent& spotlight)
	{
		//
	}

	void drawAudioSource(IMGizmo& gizmos, TransformComponent& transform, AudioSourceComponent& audioSource)
	{
		gizmos.foreground = true;
		gizmos.color = EditorGizmoColor::AUDIO_SOURCE;
		gizmos.opacity = EditorGizmoColor::AUDIO_SOURCE.a;
		gizmos.sphereWire(Transform::getPosition(transform, Space::WORLD), audioSource.maxDistance, glm::identity<glm::quat>());
	}

}