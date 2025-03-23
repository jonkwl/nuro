#include "component_gizmos.h"

#include <algorithm>
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
		if (!camera.enabled) return;

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
		if (!pointLight.enabled) return;

		gizmos.foreground = false;
		gizmos.color = pointLight.color;
		gizmos.opacity = 0.1f;
		gizmos.sphereWire(Transform::getPosition(transform, Space::WORLD), pointLight.range, glm::identity<glm::quat>());
	}

	void drawSpotlight(IMGizmo& gizmos, TransformComponent& transform, SpotlightComponent& spotlight)
	{
		if (!spotlight.enabled) return;

		//
	}

	void drawAudioSource(IMGizmo& gizmos, TransformComponent& transform, AudioSourceComponent& audioSource)
	{
		if (!audioSource.isSpatial) return;

		// Check if active listener is within range of audio source
		glm::vec3 sourcePosition = Transform::getPosition(transform, Space::WORLD);
		bool listenerIntersecting = false;
		auto audioListeners = ECS::main().view<TransformComponent, AudioListenerComponent>();
		for (auto& [entity, listenerTransform, audioListener] : audioListeners.each()) {
			if (!audioListener.enabled) break;

			glm::vec3 listenerPosition = Transform::getPosition(listenerTransform, Space::WORLD);
			glm::vec3 listenerScale = Transform::getScale(listenerTransform, Space::WORLD);
			float listenerRadius = std::max({ listenerScale.x, listenerScale.y, listenerScale.z }) * 0.5f;

			float distance = glm::distance(sourcePosition, listenerPosition);
			if ((distance - listenerRadius) > audioSource.range) break;
			
			gizmos.color = GizmoColor::RED;
			gizmos.opacity = 1.0f;
			gizmos.sphere(listenerPosition, listenerRadius);

			listenerIntersecting = true;
			break;
		}

		// Draw range of audio source
		gizmos.foreground = true;
		gizmos.color = listenerIntersecting ? EditorGizmoColor::AUDIO_SOURCE_ACTIVE : EditorGizmoColor::AUDIO_SOURCE_DEFAULT;
		gizmos.opacity = listenerIntersecting ? EditorGizmoColor::AUDIO_SOURCE_ACTIVE.a : EditorGizmoColor::AUDIO_SOURCE_DEFAULT.a;
		gizmos.sphereWire(sourcePosition, audioSource.range, glm::identity<glm::quat>());
	}

}