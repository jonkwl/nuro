//
// TEMPORARY CODE!
//

#include "game_logic.h"

#include <engine.h>

#include <iostream>

EntityContainer camera;
EntityContainer kinematic;
EntityContainer player;
EntityContainer audio;

uint32_t albedoResourceId;

void _physics_example() {
	// Get resource manager
	ResourceManager& resource = ApplicationContext::resourceManager();

	// Get ecs
	ECS& ecs = ECS::main();

	Texture* texture = new Texture();

	// Models
	const Mesh* planeMesh = Shapes::createPlane();
	const Mesh* cubeMesh = Shapes::createCube();

	auto& [sphereModelId, sphereModel] = resource.create<Model>("sphere");
	sphereModel->setSource("./resources/primitives/sphere.fbx");
	resource.exec(sphereModel->create());
	const Mesh* sphereMesh = sphereModel->queryMesh(0);

	// Standard material
	LitMaterial* standardMaterial = new LitMaterial();
	standardMaterial->baseColor = glm::vec4(glm::vec3(0.5f), 1.0f);
	standardMaterial->roughness = 0.4f;
	standardMaterial->metallic = 0.6f;

	// Red material
	LitMaterial* redMaterial = new LitMaterial();
	redMaterial->baseColor = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
	redMaterial->roughness = 0.9f;

	// Glowing material
	LitMaterial* glowingMaterial = new LitMaterial();
	glowingMaterial->emission = true;
	glowingMaterial->emissionIntensity = 4.6f;
	glowingMaterial->emissionColor = glm::vec3(0.4f, 0.4f, 1.0f);

	// Async texture loading example
	auto& [albedoId, albedo] = resource.create<Texture>("scifi-albedo");
	albedo->setSource(TextureType::ALBEDO, "./resources/example-assets/textures/sci-fi/albedo.jpg");
	resource.exec(albedo->create());

	auto& [normalId, normal] = resource.create<Texture>("scifi-nromal");
	normal->setSource(TextureType::NORMAL, "./resources/example-assets/textures/sci-fi/normal.jpg");
	resource.exec(normal->create());

	auto& [metallicId, metallic] = resource.create<Texture>("scifi-metallic");
	metallic->setSource(TextureType::METALLIC, "./resources/example-assets/textures/sci-fi/metallic.jpg");
	resource.exec(metallic->create());

	auto& [roughnessId, roughness] = resource.create<Texture>("scifi-roughness");
	roughness->setSource(TextureType::ROUGHNESS, "./resources/example-assets/textures/sci-fi/roughness.jpg");
	resource.exec(roughness->create());

	auto& [emissiveId, emissive] = resource.create<Texture>("scifi-emissive");
	emissive->setSource(TextureType::EMISSIVE, "./resources/example-assets/textures/sci-fi/emissive.jpg");
	resource.exec(emissive->create());

	auto& [heightId, height] = resource.create<Texture>("scifi-height");
	height->setSource(TextureType::HEIGHT, "./resources/example-assets/textures/sci-fi/height.jpg");
	resource.exec(height->create());

	LitMaterial* scifiMaterial = new LitMaterial();
	scifiMaterial->albedoMap = albedo;
	scifiMaterial->normalMap = normal;
	scifiMaterial->metallicMap = metallic;
	scifiMaterial->roughnessMap = roughness;
	scifiMaterial->emissiveMap = emissive;
	scifiMaterial->emission = true;
	scifiMaterial->emissionIntensity = 250.0f;

	albedoResourceId = albedoId;

	// Player material
	LitMaterial* playerMaterial = new LitMaterial();

	// Head
	EntityContainer head = ecs.createEntity("Head");

	// Camera
	camera = ecs.createEntity("Camera", head.handle());
	camera.add<CameraComponent>();

	// Directional light (sun)
	EntityContainer sun(ecs.createEntity("Sun"));
	DirectionalLightComponent& sunLight = sun.add<DirectionalLightComponent>();
	sunLight.enabled = false;
	sunLight.intensity = 0.3f;
	sunLight.color = glm::vec3(1.0, 1.0, 1.0f);

	// Sample point light
	EntityContainer pointLight(ecs.createEntity("Point Light"));
	Transform::setPosition(pointLight.transform(), glm::vec3(8.0f, 2.0f, 32.0f));
	PointLightComponent& pointLightSource = pointLight.add<PointLightComponent>();
	pointLightSource.intensity = 7.0f;
	pointLightSource.color = glm::vec3(0.5f, 0.0f, 1.0f);
	pointLightSource.range = 25.0f;
	pointLightSource.falloff = 15.0f;

	// Sample spotlight
	EntityContainer flashlight(ecs.createEntity("Flashlight", camera.handle()));
	Transform::setPosition(flashlight.transform(), glm::vec3(0.0f, 6.0f, 5.0f));
	SpotlightComponent& flashlightSource = flashlight.add<SpotlightComponent>();
	flashlightSource.intensity = 0.4f;
	flashlightSource.color = glm::vec3(1.0f, 1.0f, 1.0f);
	flashlightSource.range = 150.0f;
	flashlightSource.falloff = 10.0f;
	flashlightSource.innerAngle = 30.0f;
	flashlightSource.outerAngle = 60.0f;

	// Sample audio source
	auto& [audioClipId, audioClip] = resource.create<AudioClip>("example-audio");
	audioClip->setSource("./resources/example-assets/audio/WAV-Surround.wav");
	resource.exec(audioClip->create());
	audio = ecs.createEntity("Sound Emitter");
	Transform::setPosition(audio.transform(), glm::vec3(0.0f, 0.0f, 15.0f));
	MeshRendererComponent& audioMr = audio.add<MeshRendererComponent>();
	audioMr.mesh = sphereMesh;
	audioMr.material = glowingMaterial;
	audio.add<SphereColliderComponent>();
	RigidbodyComponent& audioRb = audio.add<RigidbodyComponent>();
	Rigidbody::setGravity(audioRb, false);
	Rigidbody::setResistance(audioRb, 1.0f);
	AudioSourceComponent& audioSource = audio.add<AudioSourceComponent>();
	AudioSource::setRange(audioSource, 50.0f);
	AudioSource::setClip(audioSource, audioClip);
	EntityContainer audioLightEntity(ecs.createEntity("Light", audio.handle()));
	PointLightComponent& audioLight = audioLightEntity.add<PointLightComponent>();
	audioLight.intensity = 6.0f;
	audioLight.range = audioSource.range;
	audioLight.color = glowingMaterial->emissionColor;

	// Ground
	EntityContainer ground(ecs.createEntity("Ground"));
	Transform::setPosition(ground.transform(), glm::vec3(0.0f, -10.1f, 35.0f));
	Transform::setScale(ground.transform(), glm::vec3(140.0f, 0.1f, 140.0f));
	MeshRendererComponent& groundMr = ground.add<MeshRendererComponent>();
	groundMr.mesh = cubeMesh;
	groundMr.material = standardMaterial;
	RigidbodyComponent& groundRb = ground.add<RigidbodyComponent>();
	Rigidbody::setKinematic(groundRb, true);
	BoxColliderComponent& groundCollider = ground.add<BoxColliderComponent>();

	// Kinematic sphere
	kinematic = ecs.createEntity("Kinematic");
	Transform::setPosition(kinematic.transform(), glm::vec3(1.0f, 0.5f, 6.0f));
	Transform::setScale(kinematic.transform(), glm::vec3(2.0f));
	MeshRendererComponent& kinematicMr = kinematic.add<MeshRendererComponent>();
	kinematicMr.mesh = sphereMesh;
	kinematicMr.material = redMaterial;
	kinematic.add<SphereColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(kinematicRb, RB_CollisionDetection::CONTINUOUS);
	Rigidbody::setKinematic(kinematicRb, true);

	// Player sphere
	player = ecs.createEntity("Player");
	Transform::setPosition(player.transform(), glm::vec3(8.0f, 0.0f, -4.0f));
	MeshRendererComponent& playerMr = player.add<MeshRendererComponent>();
	playerMr.mesh = sphereMesh;
	playerMr.material = playerMaterial;
	player.add<SphereColliderComponent>();
	RigidbodyComponent& playerRb = player.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(playerRb, RB_CollisionDetection::CONTINUOUS);
	Rigidbody::setInterpolation(playerRb, RB_Interpolation::INTERPOLATE);
	player.add<AudioListenerComponent>();

	// Player child
	EntityContainer playerChild(ecs.createEntity("Player Child", player.handle()));
	Transform::setPosition(playerChild.transform(), glm::vec3(8.0f, 2.0f, -4.5f), Space::WORLD);
	Transform::setScale(playerChild.transform(), glm::vec3(0.5f));
	MeshRendererComponent& playerChildMr = playerChild.add<MeshRendererComponent>();
	playerChildMr.mesh = sphereMesh;
	playerChildMr.material = playerMaterial;

	// Second child
	EntityContainer secondChild(ecs.createEntity("Second Child", playerChild.handle()));
	Transform::setPosition(secondChild.transform(), glm::vec3(-2.5f, 0.0f, 0.0f));
	Transform::setScale(secondChild.transform(), glm::vec3(0.6f));
	MeshRendererComponent& secondChildMr = secondChild.add<MeshRendererComponent>();
	secondChildMr.mesh = sphereMesh;
	secondChildMr.material = playerMaterial;

	// Model async loading example
	auto& [asyncModelId, asyncModel] = resource.create<Model>("mannequin");
	asyncModel->setSource("resources/example-assets/models/mannequin.fbx");
	resource.exec(asyncModel->create());
	const Mesh* asyncModelMesh = asyncModel->queryMesh(0);

	EntityContainer asyncModelEntity(ecs.createEntity("Async Model"));
	Transform::setPosition(asyncModelEntity.transform(), glm::vec3(6.0f, 0.0f, 10.0f));
	Transform::setRotation(asyncModelEntity.transform(), glm::quat(glm::radians(55.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	Transform::setScale(asyncModelEntity.transform(), glm::vec3(7.0f));
	MeshRendererComponent& asyncModelMr = asyncModelEntity.add<MeshRendererComponent>();
	asyncModelMr.mesh = asyncModelMesh;
	asyncModelMr.material = scifiMaterial;

	// Cube batch
	int objectAmount = 128;
	uint32_t c = 1;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			EntityContainer e(ecs.createEntity("Cube " + std::to_string(c)));
			Transform::setPosition(e.transform(), glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f));
			MeshRendererComponent& r = e.add<MeshRendererComponent>();
			r.mesh = cubeMesh;
			r.material = standardMaterial;
			e.add<BoxColliderComponent>();
			RigidbodyComponent& rb = e.add<RigidbodyComponent>();
			c++;
		}
	}
}

void gameSetup() {
	_physics_example();
}

void gameAwake() {
	Console::out::info("Game Start");

	AudioSourceComponent& audioSource = audio.get<AudioSourceComponent>();
	AudioSource::play(audioSource);
}

void gameQuit()
{
	Console::out::info("Game End");
}

void gameUpdate() {
	
	static bool jumped = false;
	static float zoom = -20.0f;

	float delta = Time::deltaf();

	Transform::translate(kinematic.transform(), glm::vec3(0.0f, 0.0f, 32.0f * delta));

	float force = 20.0f;
	glm::vec3 forceDirection = glm::vec3(0.0f);

	if (Input::keyDown(Key::W)) forceDirection.z = force;
	else if (Input::keyDown(Key::S)) forceDirection.z = -force;
	else forceDirection.z = 0.0;

	if (Input::keyDown(Key::A)) forceDirection.x = -force;
	else if (Input::keyDown(Key::D)) forceDirection.x = force;
	else forceDirection.x = 0.0f;

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

	float zoomStrength = 150.0f;
	glm::vec2 scrollDelta = Input::scrollDelta();
	if (scrollDelta.y > 0.0f) zoom += zoomStrength * delta;
	if (scrollDelta.y < 0.0f) zoom -= zoomStrength * delta;
	zoom = glm::clamp(zoom, -20.0f, -5.0f);
	glm::vec3 offset = glm::vec3(0.0f, 1.5f, zoom);
	Transform::setPosition(camera.transform(), glm::mix(camera.transform().position, player.transform().position + offset, 10 * delta));
}