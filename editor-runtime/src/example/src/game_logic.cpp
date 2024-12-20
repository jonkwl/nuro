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
	ECS::addMeshRendererComponent(sphere, Model::load("../resources/primitives/cube.fbx")->meshes[0]);
	ECS::addVelocityComponent(sphere);
	ECS::getRegistry().get<VelocityComponent>(sphere).intensity = 5.0f;
	TransformComponent& sphereTransform = ECS::getRegistry().get<TransformComponent>(sphere);
	sphereTransform.position.z = 5.0f;

	// Create another rendered entity
	/*cube = ECS::createEntity();
	ECS::addTransformComponent(cube);
	ECS::addMeshRendererComponent(cube, Model::load("../resources/primitives/cube.fbx")->meshes[0]);
	TransformComponent& cubeTransform = ECS::getRegistry().get<TransformComponent>(cube);
	cubeTransform.position.z = 6.5f;
	cubeTransform.scale = glm::vec3(5.0f, 5.0f, 0.25f);*/

}

glm::vec3 sphereEuler = glm::vec3(0.0f);
void update() {
	sphereEuler.y += 800.0f * Time::deltaf();
	ECS::getRegistry().get<TransformComponent>(sphere).rotation = glm::quat(glm::radians(sphereEuler));
}