#include "game_logic.h"


#include "../core/engine.h";
#include "../src/runtime/runtime.h"

#include "../core/ecs/ecs.h"

void awake() {

	// Create game camera
	entt::entity camera = ECS::createEntity();
	ECS::addTransformComponent(camera);
	ECS::addCameraComponent(camera);

	// Create some rendered entity
	entt::entity sphere = ECS::createEntity();
	ECS::addTransformComponent(sphere);
	ECS::addMeshRendererComponent(sphere, Model::load("../resources/primitives/sphere.fbx")->meshes[0]);
	TransformComponent& transform = ECS::getRegistry().get<TransformComponent>(sphere);
	transform.position.z = 5.0f;

}

void update() {
}