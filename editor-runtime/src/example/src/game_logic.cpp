#include "game_logic.h"

#include "../core/engine.h";

entity camera;
entity cube;

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	auto [camera, cameraTransform] = ECS::createEntity();
	ECS::addComponent<CameraComponent>(camera);

	auto [cube, cubeTransform] = ECS::createEntity();
	cubeTransform.position = glm::vec3(0.0f, 0.0f, 14.0f);
	ECS::addComponent<MeshRendererComponent>(cube, cubeMesh);
	ECS::addComponent<BoxColliderComponent>(cube);
	ECS::addComponent<RigidbodyComponent>(cube);

	auto [x_e1, x_tm1] = ECS::createEntity();
	x_tm1.position = glm::vec3(0.5f, 3.0f, 14.0f);
	ECS::addComponent<MeshRendererComponent>(x_e1, cubeMesh);
	ECS::addComponent<BoxColliderComponent>(x_e1);
	ECS::addComponent<RigidbodyComponent>(x_e1);

	auto [x_e2, x_tm2] = ECS::createEntity();
	x_tm2.position = glm::vec3(-0.5f, -2.0f, 14.0f);
	ECS::addComponent<MeshRendererComponent>(x_e2, cubeMesh);
	ECS::addComponent<BoxColliderComponent>(x_e2);
	ECS::addComponent<RigidbodyComponent>(x_e2);

	auto [x_e3, x_tm3] = ECS::createEntity();
	x_tm3.position = glm::vec3(0.5f, -4.0f, 13.5f);
	ECS::addComponent<MeshRendererComponent>(x_e3, cubeMesh);
	ECS::addComponent<BoxColliderComponent>(x_e3);
	ECS::addComponent<RigidbodyComponent>(x_e3);

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