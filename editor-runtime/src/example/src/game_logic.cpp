#include "game_logic.h"

#include "../core/engine.h";

#include <random>

Entity kinematicEntity;

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	LitMaterial* standardMaterial = new LitMaterial();
	standardMaterial->baseColor = glm::vec4(glm::vec3(0.25f), 1.0f);
	standardMaterial->roughness = 0.4f;

	Texture albedo = Texture::load("./src/example/assets/textures/painted-concrete/albedo.jpg", TextureType::ALBEDO);
	Texture roughness = Texture::load("./src/example/assets/textures/painted-concrete/roughness.jpg", TextureType::ROUGHNESS);
	Texture metallic = Texture::load("./src/example/assets/textures/painted-concrete/metallic.jpg", TextureType::METALLIC);
	Texture normal = Texture::load("./src/example/assets/textures/painted-concrete/normal.jpg", TextureType::NORMAL);
	LitMaterial* paintedConcrete = new LitMaterial();
	paintedConcrete->setAlbedoMap(albedo);
	paintedConcrete->setRoughnessMap(roughness);
	paintedConcrete->setMetallicMap(metallic);
	paintedConcrete->setNormalMap(normal);

	EntityContainer camera(ECS::createEntity());
	camera.add<CameraComponent>();

	EntityContainer ground(ECS::createEntity());
	ground.transform.position = glm::vec3(0.0f, -10.1f, 35.0f);
	ground.transform.scale = glm::vec3(140.0f, 0.1f, 140.0f);
	ground.add<MeshRendererComponent>(cubeMesh, standardMaterial);

	int objectAmount = 140;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			EntityContainer e(ECS::createEntity());
			e.transform.position = glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f);
			MeshRendererComponent& r = e.add<MeshRendererComponent>(cubeMesh, standardMaterial);
			e.add<BoxColliderComponent>();
			RigidbodyComponent& rb = e.add<RigidbodyComponent>();
		}
	}

	EntityContainer kinematic(ECS::createEntity());
	kinematic.transform.position = glm::vec3(12.0f, 6.0f, 100.0f);
	kinematic.transform.scale = glm::vec3(6.0f);
	kinematic.add<MeshRendererComponent>(sphereMesh, standardMaterial);
	kinematic.add<SphereColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(kinematicRb, RigidbodyComponent::CollisionDetection::CONTINUOUS);
	Rigidbody::setKinematic(kinematicRb, true);
	kinematicEntity = kinematic.root;

}

void awake() {
	Log::printProcessInfo("Game Start");
}

void quit()
{
	Log::printProcessInfo("Game End");
}

void update() {

	EntityContainer kinematic = EntityContainer(kinematicEntity);
	kinematic.transform.position += glm::vec3(0.0f, 0.0f, -32.0f * Time::deltaf());

}