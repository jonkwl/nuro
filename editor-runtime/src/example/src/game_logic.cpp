#include "game_logic.h"

#include "../core/engine.h";

Entity camera;
Entity cube;

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	camera.add<CameraComponent>();

	cube.transform.position = glm::vec3(0.0f, 0.0f, 14.0f);
	cube.add<MeshRendererComponent>(cubeMesh);
	cube.add<BoxColliderComponent>();
	cube.add<RigidbodyComponent>();

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