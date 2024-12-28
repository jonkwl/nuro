#include "game_logic.h"

#include "../core/engine.h";

Mesh* getQuadMesh() {
	// Positions for quad vertices
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);

	// Texture coordinates
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	// Normal vector (all faces pointing in the +Z direction)
	glm::vec3 nm(0.0f, 0.0f, 1.0f);

	// Calculate tangent/bitangent vectors for both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;

	// Triangle 1
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);

	// Triangle 2
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);

	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);

	// Vertex data for the quad (6 vertices, 2 triangles)
	std::vector<Mesh::VertexData> vertices = {
		// Triangle 1
		{pos1, nm, uv1, tangent1, bitangent1},
		{pos2, nm, uv2, tangent1, bitangent1},
		{pos3, nm, uv3, tangent1, bitangent1},

		// Triangle 2
		{pos1, nm, uv1, tangent2, bitangent2},
		{pos3, nm, uv3, tangent2, bitangent2},
		{pos4, nm, uv4, tangent2, bitangent2}
	};

	// Indices for the triangles (two triangles, each with 3 vertices)
	std::vector<uint32_t> indices = {
		0, 1, 2,  // Triangle 1
		3, 4, 5   // Triangle 2
	};

	// Return a new Mesh object initialized with the vertices and indices
	return new Mesh(vertices, indices);
}

Entity cameraEntity;
Entity kinematicEntity;
Entity playerEntity;
Entity planeEntity;

static bool jumped = false;
static float zoom = -20.0f;

void setup() {

	// Models
	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	// Meshes
	Mesh* planeMesh = getQuadMesh();
	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	// Standard Material
	LitMaterial* standardMaterial = new LitMaterial();
	standardMaterial->baseColor = glm::vec4(glm::vec3(0.25f), 1.0f);
	standardMaterial->roughness = 0.4f;

	// Camera
	EntityContainer camera(ECS::createEntity());
	cameraEntity = camera.root;
	camera.add<CameraComponent>();

	// Ground
	EntityContainer ground(ECS::createEntity());
	ground.transform.position = glm::vec3(0.0f, -10.1f, 35.0f);
	ground.transform.scale = glm::vec3(140.0f, 0.1f, 140.0f);
	ground.add<MeshRendererComponent>(cubeMesh, standardMaterial);

	// Cube batch
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

	// Kinematic sphere
	EntityContainer kinematic(ECS::createEntity());
	kinematicEntity = kinematic.root;
	kinematic.transform.position = glm::vec3(12.0f, 6.0f, 70.0f);
	kinematic.transform.scale = glm::vec3(6.0f);
	kinematic.add<MeshRendererComponent>(sphereMesh, standardMaterial);
	kinematic.add<SphereColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(kinematicRb, RB_CollisionDetection::CONTINUOUS);
	Rigidbody::setKinematic(kinematicRb, true);

	// Sci-Fi Plane Material
	Texture albedo = Texture::load("./src/example/assets/textures/sci-fi/albedo.jpg", TextureType::ALBEDO);
	Texture roughness = Texture::load("./src/example/assets/textures/sci-fi/roughness.jpg", TextureType::ROUGHNESS);
	Texture metallic = Texture::load("./src/example/assets/textures/sci-fi/metallic.jpg", TextureType::METALLIC);
	Texture normal = Texture::load("./src/example/assets/textures/sci-fi/normal.jpg", TextureType::NORMAL);
	Texture occlusion = Texture::load("./src/example/assets/textures/sci-fi/occlusion.jpg", TextureType::OCCLUSION);
	Texture emissive = Texture::load("./src/example/assets/textures/sci-fi/emissive.jpg", TextureType::EMISSIVE);
	Texture height = Texture::load("./src/example/assets/textures/sci-fi/height.jpg", TextureType::HEIGHT);
	LitMaterial* planeMaterial = new LitMaterial();
	planeMaterial->setAlbedoMap(albedo);
	planeMaterial->setRoughnessMap(roughness);
	planeMaterial->setMetallicMap(metallic);
	planeMaterial->setNormalMap(normal);
	planeMaterial->setOcclusionMap(occlusion);
	planeMaterial->setEmissiveMap(emissive);
	planeMaterial->emission = true;
	planeMaterial->emissionIntensity = 14.0f;
	planeMaterial->setHeightMap(height);
	planeMaterial->heightMapScale = 0.066f;

	// Test Material
	/*Texture albedoTest = Texture::load("./src/example/assets/textures/old-linoleum-floor/albedo.jpg", TextureType::ALBEDO);
	Texture roughnessTest = Texture::load("./src/example/assets/textures/old-linoleum-floor/roughness.jpg", TextureType::ROUGHNESS);
	Texture normalTest = Texture::load("./src/example/assets/textures/old-linoleum-floor/normal.jpg", TextureType::NORMAL);
	Texture occlusionTest = Texture::load("./src/example/assets/textures/old-linoleum-floor/occlusion.jpg", TextureType::OCCLUSION);
	Texture heightTest = Texture::load("./src/example/assets/textures/old-linoleum-floor/height.jpg", TextureType::HEIGHT);
	LitMaterial* testMaterial = new LitMaterial();
	testMaterial->setAlbedoMap(albedoTest);
	testMaterial->setRoughnessMap(roughnessTest);
	testMaterial->setNormalMap(normalTest);
	testMaterial->setOcclusionMap(occlusionTest);
	testMaterial->setHeightMap(heightTest);*/

	// Material Test Plane
	EntityContainer plane(ECS::createEntity());
	planeEntity = plane.root;
	plane.transform.position = glm::vec3(-24.0f, 0.0f, 18.0f);
	plane.transform.scale = glm::vec3(5.0f);
	plane.add<MeshRendererComponent>(*planeMesh, planeMaterial);

	// Player material
	LitMaterial* playerMaterial = new LitMaterial();
	/*playerMaterial->setAlbedoMap(albedo);
	playerMaterial->setRoughnessMap(roughness);
	playerMaterial->setMetallicMap(metallic);
	playerMaterial->setNormalMap(normal);
	playerMaterial->setOcclusionMap(occlusion);
	playerMaterial->setEmissiveMap(emissive);*/

	// Player sphere
	EntityContainer player(ECS::createEntity());
	playerEntity = player.root;
	player.transform.position = glm::vec3(8.0f, 0.0f, -4.0f);
	player.add<MeshRendererComponent>(sphereMesh, planeMaterial);
	player.add<SphereColliderComponent>();
	RigidbodyComponent& playerRb = player.add<RigidbodyComponent>();
	Rigidbody::setInterpolation(playerRb, RB_Interpolation::EXTRAPOLATE);
	Rigidbody::setCollisionDetection(playerRb, RB_CollisionDetection::CONTINUOUS);

	// Player child
	EntityContainer playerChild(ECS::createEntity(&player.transform));
	playerChild.transform.position = glm::vec3(0.0f, 2.0f, 0.0f);
	playerChild.transform.scale = glm::vec3(0.5f);
	playerChild.add<MeshRendererComponent>(sphereMesh, playerMaterial);

	LitMaterial::tmpPointLightPosition = player.transform.position + glm::vec3(0.0f, 2.0f, 0.0f);
	// LitMaterial::tmpPointLightPosition = glm::vec3(-22.5f, 8.0f, 16.0f);

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

	EntityContainer kinematic(kinematicEntity);
	kinematic.transform.position += glm::vec3(0.0f, 0.0f, -32.0f * delta);

	float force = 20.0f;
	glm::vec3 forceDirection = glm::vec3(0.0f);

	if (Input::keyDown(Key::W)) forceDirection.z = force;
	else if (Input::keyDown(Key::S)) forceDirection.z = -force;
	else forceDirection.z = 0.0;

	if (Input::keyDown(Key::A)) forceDirection.x = -force;
	else if (Input::keyDown(Key::D)) forceDirection.x = force;
	else forceDirection.x = 0.0f;

	EntityContainer player = EntityContainer(playerEntity);
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

	LitMaterial::tmpPointLightPosition = player.transform.position + glm::vec3(0.0f, 2.0f, 0.0f);

	EntityContainer camera(cameraEntity);
	float zoomStrength = 150.0f;
	glm::vec2 scrollDelta = Input::scrollDelta();
	if (scrollDelta.y > 0.0f) zoom += zoomStrength * delta;
	if (scrollDelta.y < 0.0f) zoom -= zoomStrength * delta;
	zoom = glm::clamp(zoom, -20.0f, -5.0f);
	glm::vec3 offset = glm::vec3(0.0f, 1.5f, zoom);
	camera.transform.position = glm::mix(camera.transform.position, player.transform.position + offset, 10 * delta);

}