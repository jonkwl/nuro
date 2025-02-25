#pragma once

#include <ecs/ecs_collection.h>
#include <rendering/postprocessing/post_processing.h>

namespace InspectableComponents
{

	//
	// INSPECTABLE ECS COMPONENTS
	//

	void drawTransform(Entity entity, TransformComponent& transform);
	void drawMeshRenderer(Entity entity, MeshRendererComponent& meshRenderer);
	void drawCamera(Entity entity, CameraComponent& camera);
	void drawDirectionalLight(Entity entity, DirectionalLightComponent& directionalLight);
	void drawPointLight(Entity entity, PointLightComponent& pointLight);
	void drawSpotlight(Entity entity, SpotlightComponent& spotlight);
	void drawVelocity(Entity entity, VelocityComponent& velocity);
	void drawBoxCollider(Entity entity, BoxColliderComponent& boxCollider);
	void drawSphereCollider(Entity entity, SphereColliderComponent& sphereCollider);
	void drawRigidbody(Entity entity, RigidbodyComponent& rigidbody);

	//
	// INSPECTABLE POST PROCESSING COMPONENTS
	//

	void drawColor(PostProcessing::Color& color);
	void drawMotionBlur(PostProcessing::MotionBlur& motionBlur);
	void drawBloom(PostProcessing::Bloom& bloom);
	void drawChromaticAberration(PostProcessing::ChromaticAberration& chromaticAberration);
	void drawVignette(PostProcessing::Vignette& vignette);
	void drawAmbientOcclusion(PostProcessing::AmbientOcclusion& ambientOcclusion);

};