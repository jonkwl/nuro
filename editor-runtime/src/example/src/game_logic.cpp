#include "game_logic.h"

#include "../core/engine.h";

#include <random>

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	DebugMaterial metalMaterial{ 1, 1, "Lit Shader", "Metal Material" };
	DebugMaterial plasticMaterial{ 1, 2, "Lit Shader", "Plastic Material" };
	DebugMaterial bulletMaterial{ 1, 3, "Lit Shader", "Bullet Material" };
	DebugMaterial skyMaterial{ 1, 4, "Lit Shader", "Sky Material" };
	DebugMaterial boxMaterial{ 1, 5, "Lit Shader", "Box Material" };
	DebugMaterial glassMaterial{ 2, 6, "Transparent Shader", "Glass Material" };
	DebugMaterial waterMaterial{ 2, 7, "Transparent Shader", "Water Material" };
	DebugMaterial bottleMaterial{ 2, 8, "Transparent Shader", "Bottle Material" };
	DebugMaterial customMaterial{ 3, 9, "Custom Shader", "Custom Material" };
	DebugMaterial magicMaterial{ 3, 10, "Custom Shader", "Magic Material" };
	DebugMaterial materials[10] = { metalMaterial, plasticMaterial, bulletMaterial, skyMaterial, boxMaterial, glassMaterial, waterMaterial, bottleMaterial, customMaterial, magicMaterial };

	EntityContainer camera = ECS::createEntity();
	camera.add<CameraComponent>();

	EntityContainer ground = ECS::createEntity();
	ground.transform.position = glm::vec3(0.0f, -10.1f, 35.0f);
	ground.transform.scale = glm::vec3(140.0f, 0.1f, 140.0f);
	ground.add<MeshRendererComponent>(cubeMesh);

	EntityContainer sphere = ECS::createEntity();
	sphere.transform.position = glm::vec3(3.0f, 5.0f, 19.0f);
	sphere.add<MeshRendererComponent>(sphereMesh);
	sphere.add<SphereColliderComponent>();
	sphere.add<RigidbodyComponent>();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, 9);

	int objectAmount = 200;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			EntityContainer e = ECS::createEntity();
			e.transform.position = glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f);
			MeshRendererComponent& r = e.add<MeshRendererComponent>(cubeMesh);
			e.add<BoxColliderComponent>();
			e.add<RigidbodyComponent>();

			r.material = materials[distrib(gen)];
		}
	}

	EntityContainer noGravity = ECS::createEntity();
	noGravity.transform.position = glm::vec3(3.0f, 6.0f, 16.0f);
	noGravity.add<MeshRendererComponent>(cubeMesh);
	noGravity.add<BoxColliderComponent>();
	RigidbodyComponent& noGravityRb = noGravity.add<RigidbodyComponent>();
	Rigidbody::setGravity(noGravityRb, false);

	EntityContainer kinematic = ECS::createEntity();
	kinematic.transform.position = glm::vec3(3.0f, 2.5f, 16.0f);
	kinematic.add<MeshRendererComponent>(cubeMesh);
	kinematic.add<BoxColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setKinematic(kinematicRb, true);

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