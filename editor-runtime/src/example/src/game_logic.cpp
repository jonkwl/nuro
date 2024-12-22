#include "game_logic.h"

#include "../core/engine.h";

entity camera;
entity cube;

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	camera = ECS::createEntity();
	ECS::addComponent<TransformComponent>(camera);
	ECS::addComponent<CameraComponent>(camera);

	entity cube = ECS::createEntity();
	TransformComponent& cubeTransform = ECS::addComponent<TransformComponent>(cube);
	ECS::addComponent<MeshRendererComponent>(cube, cubeMesh);
	cubeTransform.position = glm::vec3(0.0f, 0.0f, 14.0f);
	ECS::addComponent<BoxColliderComponent>(cube);
	ECS::addComponent<RigidbodyComponent>(cube);

}

void awake() {
	Log::printProcessInfo("Game Start");
}

void quit()
{
	Log::printProcessInfo("Game End");
}

void update() {
}