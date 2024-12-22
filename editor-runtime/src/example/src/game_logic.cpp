#include "game_logic.h"

#include "../core/engine.h";
#include "../src/runtime/runtime.h"

uint32_t camera;
uint32_t cube;
uint32_t sphere;

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	camera = ECS::createEntity();
	ECS::addComponent<TransformComponent>(camera);
	ECS::addComponent<CameraComponent>(camera);

	cube = ECS::createEntity();
	TransformComponent& cubeTransform = ECS::addComponent<TransformComponent>(cube);
	MeshRendererComponent& cubeRenderer = ECS::addComponent<MeshRendererComponent>(cube, cubeMesh);
	cubeTransform.position.z = 10.0f;
	cubeTransform.scale = glm::vec3(8.0f, 8.0f, 0.1f);

	sphere = ECS::createEntity();
	TransformComponent& sphereTransform = ECS::addComponent<TransformComponent>(sphere);
	MeshRendererComponent& sphereRenderer = ECS::addComponent<MeshRendererComponent>(sphere, sphereMesh);
	sphereTransform.position.z = 8.0f;

}

void awake() {
	Log::printProcessInfo("Game Start");
}

void quit()
{
	Log::printProcessInfo("Game End");
}

void update() {
	float delta = Time::deltaf();

	TransformComponent& sphereTransform = ECS::getComponent<TransformComponent>(sphere);
	Transform::rotateY(sphereTransform, 360.0f * delta);
}