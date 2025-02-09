#pragma once

#include "../src/core/ecs/ecs_collection.h"

namespace InspectableComponents
{

	void drawTransform(TransformComponent& transform);
	void drawMeshRenderer(MeshRendererComponent& meshRenderer);
	void drawCamera(CameraComponent& camera);
	void drawDirectionalLight(DirectionalLightComponent& directionalLight);
	void drawPointLight(PointLightComponent& pointLight);
	void drawSpotlight(SpotlightComponent& spotlight);
	void drawVelocity(VelocityComponent& velocity);
	void drawBoxCollider(BoxColliderComponent& boxCollider);
	void drawSphereCollider(SphereColliderComponent& sphereCollider);
	void drawRigidbody(RigidbodyComponent& rigidbody);

};