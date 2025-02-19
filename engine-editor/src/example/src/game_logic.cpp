//
// TEMPORARY CODE!
//

#include "game_logic.h"

#include "../src/core/engine.h"

#include <iostream>

Entity cameraEntity;
Entity kinematicEntity;
Entity playerEntity;
Entity planeEntity;

void _physics_example() {
	// Get loader
	ResourceLoader& loader = ApplicationContext::getResourceLoader();

	// Models
	Model* cubeModel = new Model();
	cubeModel->setSource("../resources/primitives/cube.fbx");
	loader.createSync(cubeModel);
	const Mesh* cubeMesh = cubeModel->queryMesh(0);

	Model* sphereModel = new Model();
	sphereModel->setSource("../resources/primitives/sphere.fbx");
	loader.createSync(sphereModel);
	const Mesh* sphereMesh = sphereModel->queryMesh(0);

	// Standard Material
	LitMaterial* standardMaterial = new LitMaterial();
	standardMaterial->baseColor = glm::vec4(glm::vec3(0.5f), 1.0f);
	standardMaterial->roughness = 0.4f;

	// Sci-Fi Plane Material
	LitMaterial* planeMaterial = new LitMaterial();

	// Cube batch
	int objectAmount = 16;
	uint32_t c = 1;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			EntityContainer e("Cube " + std::to_string(c), ECS::createEntity());
			e.transform.position = glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f);
			MeshRendererComponent& r = e.add<MeshRendererComponent>(cubeMesh, standardMaterial);
			e.add<BoxColliderComponent>();
			RigidbodyComponent& rb = e.add<RigidbodyComponent>();
			c++;
		}
	}

	// Directional light (sun)
	EntityContainer sun("Sun", ECS::createEntity());
	DirectionalLightComponent& sunLight = sun.add<DirectionalLightComponent>();
	sunLight.intensity = 0.3f;
	sunLight.color = glm::vec3(1.0, 1.0, 1.0f);

	// Sample point light
	EntityContainer pointLight("Point Light", ECS::createEntity());
	pointLight.transform.position = glm::vec3(0.0f, 2.0f, 45.0f);
	PointLightComponent& pointLightSource = pointLight.add<PointLightComponent>();
	pointLightSource.intensity = 3.0f;
	pointLightSource.color = glm::vec3(0.5f, 0.0f, 1.0f);
	pointLightSource.range = 25.0f;
	pointLightSource.falloff = 15.0f;

	// Sample spotlight
	EntityContainer flashlight("Flashlight", ECS::createEntity());
	flashlight.transform.position = glm::vec3(0.0f, 6.0f, 5.0f);
	SpotlightComponent& flashlightSource = flashlight.add<SpotlightComponent>();
	flashlightSource.intensity = 10.0f;
	flashlightSource.color = glm::vec3(1.0f, 1.0f, 1.0f);
	flashlightSource.range = 150.0f;
	flashlightSource.falloff = 10.0f;
	flashlightSource.innerAngle = 45.0f;
	flashlightSource.outerAngle = 90.0f;

	// Camera
	EntityContainer camera("Camera", ECS::createEntity());
	cameraEntity = camera.root;
	camera.transform.position = glm::vec3(5.0f, 0.0f, 0.0f);
	camera.add<CameraComponent>();

	// Ground
	EntityContainer ground("Ground", ECS::createEntity());
	ground.transform.position = glm::vec3(0.0f, -10.1f, 35.0f);
	ground.transform.scale = glm::vec3(140.0f, 0.1f, 140.0f);
	ground.add<MeshRendererComponent>(cubeMesh, standardMaterial);

	// Kinematic sphere
	EntityContainer kinematic("Kinematic", ECS::createEntity());
	kinematicEntity = kinematic.root;
	kinematic.transform.position = glm::vec3(12.0f, 6.0f, -5.0f);
	kinematic.transform.scale = glm::vec3(6.0f);
	kinematic.add<MeshRendererComponent>(sphereMesh, standardMaterial);
	kinematic.add<SphereColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(kinematicRb, RB_CollisionDetection::CONTINUOUS);
	Rigidbody::setKinematic(kinematicRb, true);

	// Player material
	LitMaterial* playerMaterial = new LitMaterial();

	// Player sphere
	EntityContainer player("Player", ECS::createEntity());
	playerEntity = player.root;
	player.transform.position = glm::vec3(8.0f, 0.0f, -4.0f);
	player.add<MeshRendererComponent>(sphereMesh, playerMaterial);
	player.add<SphereColliderComponent>();
	RigidbodyComponent& playerRb = player.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(playerRb, RB_CollisionDetection::CONTINUOUS);

	// Player child
	EntityContainer playerChild("Player Child", ECS::createEntity(&player.transform));
	playerChild.transform.position = glm::vec3(0.0f, 2.0f, 0.0f);
	playerChild.transform.scale = glm::vec3(0.5f);
	playerChild.add<MeshRendererComponent>(sphereMesh, playerMaterial);

	// Async asset loading test
	Texture* albedo = new Texture();
	albedo->setSource(TextureType::ALBEDO, "./src/example/assets/textures/sci-fi/albedo.jpg");
	loader.createAsync(albedo);

	Texture* normal = new Texture();
	normal->setSource(TextureType::NORMAL, "./src/example/assets/textures/sci-fi/normal.jpg");
	loader.createAsync(normal);

	Texture* metallic = new Texture();
	metallic->setSource(TextureType::METALLIC, "./src/example/assets/textures/sci-fi/metallic.jpg");
	loader.createAsync(metallic);

	Texture* roughness = new Texture();
	roughness->setSource(TextureType::ROUGHNESS, "./src/example/assets/textures/sci-fi/roughness.jpg");
	loader.createAsync(roughness);

	Texture* emissive = new Texture();
	emissive->setSource(TextureType::EMISSIVE, "./src/example/assets/textures/sci-fi/emissive.jpg");
	loader.createAsync(emissive);

	Texture* occlusion = new Texture();
	occlusion->setSource(TextureType::OCCLUSION, "./src/example/assets/textures/sci-fi/occlusion.jpg");
	loader.createAsync(occlusion);

	LitMaterial* cubeMaterial = new LitMaterial();
	cubeMaterial->albedoMap = albedo;
	cubeMaterial->normalMap = normal;
	cubeMaterial->metallicMap = metallic;
	cubeMaterial->roughnessMap = roughness;
	cubeMaterial->emissiveMap = emissive;
	cubeMaterial->emission = true;
	cubeMaterial->emissionIntensity = 250.0f;
	cubeMaterial->occlusionMap = occlusion;

	// Model async loading example
	Model* asyncModel = new Model();
	asyncModel->setSource("./src/example/assets/models/mannequin.fbx");
	loader.createAsync(asyncModel);

	EntityContainer e("Async Model", ECS::createEntity());
	e.transform.position = glm::vec3(8.0f, 3.0f, 10.0f);
	e.transform.rotation = glm::quat(glm::radians(55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	e.transform.scale = glm::vec3(7.0f);
	e.add<MeshRendererComponent>(asyncModel->queryMesh(0), cubeMaterial);
}

void gameSetup() {
	_physics_example();
}

void gameAwake() {
	Console::out::processInfo("Game Start");
}

void gameQuit()
{
	Console::out::processInfo("Game End");
}

void gameUpdate() {
	
	static bool jumped = false;
	static float zoom = -20.0f;

	float delta = Time::deltaf();

	EntityContainer kinematic("Kinematic", kinematicEntity);
	kinematic.transform.position += glm::vec3(0.0f, 0.0f, 32.0f * delta);

	float force = 20.0f;
	glm::vec3 forceDirection = glm::vec3(0.0f);

	if (Input::keyDown(Key::W)) forceDirection.z = force;
	else if (Input::keyDown(Key::S)) forceDirection.z = -force;
	else forceDirection.z = 0.0;

	if (Input::keyDown(Key::A)) forceDirection.x = -force;
	else if (Input::keyDown(Key::D)) forceDirection.x = force;
	else forceDirection.x = 0.0f;

	EntityContainer player = EntityContainer("Player", playerEntity);
	RigidbodyComponent& playerRb = player.get<RigidbodyComponent>();
	Rigidbody::addForce(playerRb, forceDirection);

	float jumpForce = 8.0f;
	if (Input::mouseDown(MouseButton::LEFT) && !jumped) {
		Rigidbody::addForce(playerRb, glm::vec3(0.0f, jumpForce, 0.0f), RB_ForceMode::IMPULSE);
		jumped = true;
	}
	if (!Input::mouseDown(MouseButton::LEFT)) {
		jumped = false;
	}

	EntityContainer camera("Camera", cameraEntity);
	float zoomStrength = 150.0f;
	glm::vec2 scrollDelta = Input::scrollDelta();
	if (scrollDelta.y > 0.0f) zoom += zoomStrength * delta;
	if (scrollDelta.y < 0.0f) zoom -= zoomStrength * delta;
	zoom = glm::clamp(zoom, -20.0f, -5.0f);
	glm::vec3 offset = glm::vec3(0.0f, 1.5f, zoom);
	camera.transform.position = glm::mix(camera.transform.position, player.transform.position + offset, 10 * delta);

}