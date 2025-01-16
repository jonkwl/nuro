#include "game_logic.h"

#include "../core/engine.h"

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

void _example() {

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
	planeMaterial->emissionIntensity = 11.0f;
	//planeMaterial->setHeightMap(height);
	//planeMaterial->heightMapScale = 0.066f;

	// Directional light (sun)
	/*EntityContainer sun("Sun", ECS::createEntity());
	DirectionalLightComponent& sunLight = sun.add<DirectionalLightComponent>();
	sunLight.intensity = 0.7f;
	sunLight.color = glm::vec3(1.0, 1.0, 1.0f);

	// Sample point light
	EntityContainer pointLight("Point Light", ECS::createEntity());
	pointLight.transform.position = glm::vec3(0.0f, 2.0f, 45.0f);
	PointLightComponent& pointLightSource = pointLight.add<PointLightComponent>();
	pointLightSource.intensity = 5.0f;
	pointLightSource.color = glm::vec3(0.5f, 0.0f, 1.0f);
	pointLightSource.range = 25.0f;
	pointLightSource.falloff = 15.0f;*/

	// Sample spotlight
	EntityContainer flashlight("Flashlight", ECS::createEntity());
	flashlight.transform.position = glm::vec3(0.0f, 6.0f, 5.0f);
	SpotlightComponent& flashlightSource = flashlight.add<SpotlightComponent>();
	flashlightSource.intensity = 50.0f;
	flashlightSource.color = glm::vec3(1.0f, 1.0f, 1.0f);
	flashlightSource.range = 50.0f;
	flashlightSource.falloff = 10.0f;
	flashlightSource.innerAngle = 0.0f;
	flashlightSource.outerAngle = 45.0f;

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

	// Cube batch
	int objectAmount = 250;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			EntityContainer e("Cube", ECS::createEntity());
			e.transform.position = glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f);
			MeshRendererComponent& r = e.add<MeshRendererComponent>(cubeMesh, standardMaterial);
			r.material = planeMaterial;
			e.add<BoxColliderComponent>();
			RigidbodyComponent& rb = e.add<RigidbodyComponent>();
		}
	}

	// Kinematic sphere
	EntityContainer kinematic("Kinematic", ECS::createEntity());
	kinematicEntity = kinematic.root;
	kinematic.transform.position = glm::vec3(12.0f, 6.0f, 70.0f);
	kinematic.transform.scale = glm::vec3(6.0f);
	kinematic.add<MeshRendererComponent>(sphereMesh, standardMaterial);
	kinematic.add<SphereColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(kinematicRb, RB_CollisionDetection::CONTINUOUS);
	Rigidbody::setKinematic(kinematicRb, true);

	// Material Test Plane
	/*EntityContainer plane("Plane", ECS::createEntity());
	planeEntity = plane.root;
	plane.transform.position = glm::vec3(-24.0f, 0.0f, 18.0f);
	plane.transform.scale = glm::vec3(5.0f);
	plane.add<MeshRendererComponent>(*planeMesh, planeMaterial);*/

	// Player material
	LitMaterial* playerMaterial = new LitMaterial();

	// Player sphere
	EntityContainer player("Player", ECS::createEntity());
	playerEntity = player.root;
	player.transform.position = glm::vec3(8.0f, 0.0f, -4.0f);
	player.add<MeshRendererComponent>(sphereMesh, playerMaterial);
	player.add<SphereColliderComponent>();
	RigidbodyComponent& playerRb = player.add<RigidbodyComponent>();
	// Rigidbody::setInterpolation(playerRb, RB_Interpolation::EXTRAPOLATE);
	Rigidbody::setCollisionDetection(playerRb, RB_CollisionDetection::CONTINUOUS);

	// Player child
	EntityContainer playerChild("Player Child", ECS::createEntity(&player.transform));
	playerChild.transform.position = glm::vec3(0.0f, 2.0f, 0.0f);
	playerChild.transform.scale = glm::vec3(0.5f);
	playerChild.add<MeshRendererComponent>(sphereMesh, playerMaterial);

	entt::entity invaliddd = entt::entity();
	EntityContainer x("Some", invaliddd);
	x.add<TransformComponent>();
	x.remove<TransformComponent>();

	EntityContainer playerEC("Player", player.root);
	playerEC.get<VelocityComponent>();
	playerEC.remove<TransformComponent>();

}

void setup() {
	_example();
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

	EntityContainer kinematic("Kinematic", kinematicEntity);
	kinematic.transform.position += glm::vec3(0.0f, 0.0f, -32.0f * delta);

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