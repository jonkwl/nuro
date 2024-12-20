#include "game_logic.h"

#include "../core/engine.h";
#include "../src/runtime/runtime.h"

#include "../core/ecs/ecs.h"

entt::entity camera;
entt::entity sphere;
entt::entity cube;

void awake() {

	// Create game camera
	camera = ECS::createEntity();
	ECS::addTransformComponent(camera);
	ECS::addCameraComponent(camera);

	// Create some rendered entity
	sphere = ECS::createEntity();
	ECS::addTransformComponent(sphere);
	ECS::addMeshRendererComponent(sphere, Model::load("../resources/primitives/sphere.fbx")->meshes[0]);
	TransformComponent& sphereTransform = ECS::getRegistry().get<TransformComponent>(sphere);
	sphereTransform.position.z = 10.0f;

	// Create another rendered entity
	cube = ECS::createEntity();
	ECS::addTransformComponent(cube);
	ECS::addMeshRendererComponent(cube, Model::load("../resources/primitives/cube.fbx")->meshes[0]);
	TransformComponent& cubeTransform = ECS::getRegistry().get<TransformComponent>(cube);
	cubeTransform.position.z = 12.0f;
	cubeTransform.scale = glm::vec3(5.0f, 5.0f, 0.25f);

}

void update() {
}