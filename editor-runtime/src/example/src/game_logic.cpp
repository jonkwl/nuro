#include "game_logic.h"

#include "../core/engine.h";

Entity camera;
Entity ground;
Entity sphere;

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	camera.add<CameraComponent>();

	ground.transform.position = glm::vec3(0.0f, -10.1f, 35.0f);
	ground.transform.scale = glm::vec3(140.0f, 0.1f, 140.0f);
	ground.add<MeshRendererComponent>(cubeMesh);

	sphere.transform.position = glm::vec3(3.0f, 5.0f, 19.0f);
	sphere.add<MeshRendererComponent>(sphereMesh);
	sphere.add<SphereColliderComponent>();
	sphere.add<RigidbodyComponent>();

	int objectAmount = 200;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			Entity e;
			e.transform.position = glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f);
			e.add<MeshRendererComponent>(cubeMesh);
			e.add<BoxColliderComponent>();
			e.add<RigidbodyComponent>();
		}
	}

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

	// camera.transform.position = glm::mix(camera.transform.position, target.transform.position + glm::vec3(0.0f, 0.0f, -5.0f), 10.0f * delta);
	// camera.transform.rotation = glm::slerp(camera.transform.rotation, Transform::fromEuler(0.0f, 0.0f, 0.0f), 10.0f * delta);
}