#include "game_logic.h"

#include <iostream>

#include "../src/engine.h"
#include "../src/runtime/runtime.h"

Entity* mannequin = nullptr;
LitMaterial* neon = nullptr;
Entity* neonLight = nullptr;
void defaultScene() {
	// Post Processing Lens Dirt
	Texture lensDirt = Texture::load("./user/assets/textures/lens_dirt.jpg", TextureType::ALBEDO);
	PostProcessing::bloom.lensDirtTexture = lensDirt;
	PostProcessing::bloom.lensDirtEnabled = true;

	// Create all entities
	LitMaterial* sphereMaterial = new LitMaterial();
	sphereMaterial->roughness = 0.15f;
	Model* sphereModel = Model::load("./user/assets/models/sphere.fbx");
	Entity* sphere = Runtime::createEntity();
	sphere->meshRenderer.model = sphereModel;
	sphere->meshRenderer.materials.push_back(sphereMaterial);
	sphere->transform.position = glm::vec3(0.0f, 0.6f, 6.5f);

	Entity* collisionSphere = Runtime::createEntity();
	collisionSphere->meshRenderer.model = sphereModel;
	collisionSphere->meshRenderer.materials.push_back(sphereMaterial);
	collisionSphere->transform.position = glm::vec3(5.0f, 0.6f, 6.5f);

	Texture sphereAlbedo = Texture::load("./user/assets/textures/mat_albedo.jpg", TextureType::ALBEDO);
	Texture sphereRoughness = Texture::load("./user/assets/textures/mat_roughness.jpg", TextureType::ROUGHNESS);
	Texture sphereMetallic = Texture::load("./user/assets/textures/mat_metallic.jpg", TextureType::METALLIC);
	Texture sphereNormal = Texture::load("./user/assets/textures/mat_normal.jpg", TextureType::NORMAL);
	LitMaterial* pbrSphereMaterial = new LitMaterial();
	pbrSphereMaterial->setAlbedoMap(sphereAlbedo);
	pbrSphereMaterial->setRoughnessMap(sphereRoughness);
	pbrSphereMaterial->setMetallicMap(sphereMetallic);
	pbrSphereMaterial->setNormalMap(sphereNormal);
	Model* pbrSphereModel = Model::load("./user/assets/models/sphere.fbx");
	Entity* pbrSphere = Runtime::createEntity();
	pbrSphere->meshRenderer.model = pbrSphereModel;
	pbrSphere->meshRenderer.materials.push_back(pbrSphereMaterial);
	pbrSphere->transform.position = glm::vec3(3.0f, 0.6f, 6.5f);

	Texture plankAlbedo = Texture::load("./user/assets/textures/plank.jpg", TextureType::ALBEDO);
	LitMaterial* plank = new LitMaterial();
	plank->tiling = glm::vec2(2.0f, 2.0f);
	plank->setAlbedoMap(plankAlbedo);
	plank->roughness = 0.0f;
	plank->metallic = 0.0f;
	Model* cubeModel = Model::load("./user/assets/models/cube.fbx");
	Entity* cube = Runtime::createEntity();
	cube->meshRenderer.model = cubeModel;
	cube->meshRenderer.materials.push_back(plank);
	cube->transform.position = glm::vec3(-3.0f, 1.0f, 6.5f);

	LitMaterial* floorMaterial = new LitMaterial();
	floorMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	floorMaterial->roughness = 0.35f;
	Model* floorModel = Model::load("./user/assets/models/cube.fbx");
	floorModel->castsShadow = false;
	Entity* floor = Runtime::createEntity();
	floor->meshRenderer.model = floorModel;
	floor->meshRenderer.materials.push_back(floorMaterial);
	floor->transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
	floor->transform.scale = glm::vec3(150.0f, 0.1f, 150.0f);

	LitMaterial* wallMaterial = new LitMaterial();
	wallMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	wallMaterial->roughness = 0.35f;
	Model* wallModel = Model::load("./user/assets/models/cube.fbx");
	wallModel->castsShadow = true;
	Entity* wall = Runtime::createEntity();
	wall->meshRenderer.model = wallModel;
	wall->meshRenderer.materials.push_back(wallMaterial);
	wall->transform.position = glm::vec3(0.0f, -1.0f, 10.0f);
	wall->transform.scale = glm::vec3(7.0f, 5.0f, 0.1f);

	Texture smearedWallAlbedo = Texture::load("./user/assets/textures/Smeared Wall_BaseColor.jpg", TextureType::ALBEDO);
	Texture smearedWallRoughness = Texture::load("./user/assets/textures/Smeared Wall_Roughness.jpg", TextureType::ROUGHNESS);
	Texture smearedWallNormal = Texture::load("./user/assets/textures/Smeared Wall_Normal.jpg", TextureType::NORMAL);
	LitMaterial* smearedWallMaterial = new LitMaterial();
	smearedWallMaterial->baseColor = glm::vec4(1.0f, 0.96f, 0.86f, 1.0f);
	smearedWallMaterial->setAlbedoMap(smearedWallAlbedo);
	smearedWallMaterial->roughness = 0.48f;
	smearedWallMaterial->setMetallicMap(smearedWallRoughness);
	smearedWallMaterial->setNormalMap(smearedWallNormal);
	smearedWallMaterial->tiling = glm::vec2(20.0f, 10.0f);
	Model* smearedWallModel = Model::load("./user/assets/models/cube.fbx");
	Entity* smearedWall = Runtime::createEntity();
	smearedWall->meshRenderer.model = smearedWallModel;
	smearedWall->meshRenderer.materials.push_back(smearedWallMaterial);
	smearedWall->transform.position = glm::vec3(20.0f, -1.0f, 10.0f);
	smearedWall->transform.scale = glm::vec3(10.0f, 5.0f, 0.1f);

	Entity* smearedWall2 = Runtime::createEntity();
	smearedWall2->meshRenderer.model = smearedWallModel;
	smearedWall2->meshRenderer.materials.push_back(smearedWallMaterial);
	smearedWall2->transform.position = glm::vec3(23.0f, -1.0f, 10.0f);
	smearedWall2->transform.rotation = glm::vec3(0.0, 80.0f, 0.0f);
	smearedWall2->transform.scale = glm::vec3(10.0f, 5.0f, 0.1f);

	Texture mannequinAlbedo = Texture::load("./user/assets/textures/mannequin_albedo.jpg", TextureType::ALBEDO);
	Texture mannequinRoughness = Texture::load("./user/assets/textures/mannequin_roughness.jpg", TextureType::ROUGHNESS);
	Texture mannequinMetallic = Texture::load("./user/assets/textures/mannequin_metallic.jpg", TextureType::METALLIC);
	LitMaterial* mannequinMaterial = new LitMaterial();
	mannequinMaterial->setAlbedoMap(mannequinAlbedo);
	mannequinMaterial->setRoughnessMap(mannequinRoughness);
	mannequinMaterial->setMetallicMap(mannequinMetallic);
	mannequinMaterial->baseColor = glm::vec4(0.5f, 0.1f, 0.1f, 1.0f);
	Model* mannequinModel = Model::load("./user/assets/models/mannequin.fbx");
	mannequin = Runtime::createEntity();
	mannequin->meshRenderer.model = mannequinModel;
	mannequin->meshRenderer.materials.push_back(mannequinMaterial);
	mannequin->meshRenderer.useMotionBlur = true;
	mannequin->meshRenderer.motionBlurIntensity = 3.0f;
	mannequin->transform.position = glm::vec3(14.0f, -0.9f, 6.1f);
	mannequin->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	mannequin->transform.scale = glm::vec3(1.4f);

	LitMaterial* whiteMaterial = new LitMaterial();
	whiteMaterial->baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	whiteMaterial->roughness = 0.15f;
	Model* shadowedSphereModel = Model::load("./user/assets/models/sphere.fbx");
	Entity* shadowedSphere = Runtime::createEntity();
	shadowedSphere->meshRenderer.model = shadowedSphereModel;
	shadowedSphere->meshRenderer.materials.push_back(whiteMaterial);
	shadowedSphere->transform.position = glm::vec3(12.5f, 0.6f, 8.0f);

	neon = new LitMaterial();
	neon->emission = true;
	neon->emissionColor = glm::vec4(0.5f, 0.2f, 1.0f, 1.0f);
	neon->emissionIntensity = 1.2f;

	Model* neonModel = Model::load("./user/assets/models/pacman_neon_lights.fbx");
	neonLight = Runtime::createEntity();
	neonLight->transform.position = glm::vec3(14.5f, 2.5f, 9.5f);
	neonLight->transform.scale = glm::vec3(5.0f, 5.0f, 5.0f);
	neonLight->meshRenderer.model = neonModel;
	neonLight->meshRenderer.materials = { neon, neon, neon, neon, neon, neon, neon };

	/*Model* dragonModel = Model::load("./user/assets/models/dragon.fbx");
	LitMaterial* dragonMaterial = new LitMaterial();
	dragonMaterial->baseColor = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
	dragonMaterial->roughness = 0.4f;
	
	Entity* dragon = Runtime::createEntity();
	dragon->transform.position = glm::vec3(9.7f, 1.0f, 0.0f);
	dragon->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	dragon->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	dragon->meshRenderer.model = dragonModel;
	std::vector<IMaterial*> dragonMats(100, dragonMaterial);
	dragon->meshRenderer.materials = dragonMats;*/

	/*Entity* dragon2 = Runtime::createEntity();
	dragon2->transform.position = glm::vec3(32.0f, 1.0f, 0.0f);
	dragon2->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	dragon2->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	dragon2->meshRenderer.model = dragonModel;
	dragon2->meshRenderer.materials = dragonMats;

	Entity* dragon3 = Runtime::createEntity();
	dragon3->transform.position = glm::vec3(37.0f, 1.0f, 0.0f);
	dragon3->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	dragon3->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	dragon3->meshRenderer.model = dragonModel;
	dragon3->meshRenderer.materials = dragonMats;

	Entity* dragon4 = Runtime::createEntity();
	dragon4->transform.position = glm::vec3(42.0f, 1.0f, 0.0f);
	dragon4->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	dragon4->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	dragon4->meshRenderer.model = dragonModel;
	dragon4->meshRenderer.materials = dragonMats;

	Entity* dragon5 = Runtime::createEntity();
	dragon5->transform.position = glm::vec3(47.0f, 1.0f, 0.0f);
	dragon5->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	dragon5->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	dragon5->meshRenderer.model = dragonModel;
	dragon5->meshRenderer.materials = dragonMats;

	Entity* dragon6 = Runtime::createEntity();
	dragon6->transform.position = glm::vec3(52.0f, 1.0f, 0.0f);
	dragon6->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	dragon6->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	dragon6->meshRenderer.model = dragonModel;
	dragon6->meshRenderer.materials = dragonMats;*/

	/*Model* audiModel = Model::load("./user/assets/models/audi.fbx");
	LitMaterial* audiMaterial = new LitMaterial();
	audiMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	audiMaterial->roughness = 0.2f;
	audiMaterial->metallic = 1.0f;
	Entity* audi = Runtime::createEntity();
	audi->transform.position = glm::vec3(-8.0f, 1.0f, 0.0f);
	audi->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	audi->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	audi->meshRenderer.model = audiModel;
	std::vector<IMaterial*> audiMats(100, audiMaterial);
	audi->meshRenderer.materials = audiMats;*/

	/*for (int i = 0; i < 40; i++) {
		Entity* x = Runtime::createEntity();
		x = Runtime::createEntity();
		x->meshRenderer.model = mannequinModel;
		x->meshRenderer.materials.push_back(mannequinMaterial);
		x->transform.position = glm::vec3(14.0f, -0.9f, 8.5f);
		x->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
		x->transform.scale = glm::vec3(1.4f);

		x->transform.position.x += 3.0f * (i + 1);
	}*/
}

std::vector<Entity*> cubes = std::vector<Entity*>();
void performanceScene() {
	int gridX = 5;
	int gridZ = 5;
	float offset = 3.0f;

	Model* cube = Model::load("./user/assets/models/cube.fbx");
	
	LitMaterial* material = new LitMaterial();
	material->baseColor = glm::vec4(1.0f);
	material->roughness = 0.1f;
	material->metallic = 0.0f;

	PostProcessing::bloom.intensity = 0.25f;

	glm::vec2 halfSize = glm::vec2(gridX * offset, gridZ * offset);

	for (int x = 1; x < gridX + 1; x++) {
		for (int z = 1; z < gridZ + 1; z++) {
			Entity* entity = Runtime::createEntity();
			entity = Runtime::createEntity();
			entity->meshRenderer.model = cube;
			entity->meshRenderer.materials.push_back(material);
			entity->transform.position = glm::vec3(14.0f, -0.9f, 8.5f);
			entity->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
			entity->transform.scale = glm::vec3(1.0f);
			entity->transform.position.x += offset * x - halfSize.x;
			entity->transform.position.y = -3.5f;
			entity->transform.position.z += offset * z - halfSize.y;
			cubes.push_back(entity);
		}
	}

	Entity* entity = Runtime::createEntity();
	entity = Runtime::createEntity();
	entity->meshRenderer.model = cube;
	entity->meshRenderer.materials.push_back(material);
	entity->transform.position = glm::vec3(0.0f, 1.5f, 4.0f);
	entity->transform.scale.x = 2.0f;
	cubes.push_back(entity);
}

void awake() {
	defaultScene();
}

void update() {
	// Performance Scene
	/*
	float range = 1.0f;
	for (int i = 0; i < cubes.size(); i++) {
		cubes[i]->transform.position.y = range * std::sin(Runtime::time * (float)i);
	}
	cubes[cubes.size() - 1]->transform.rotation.z += 360.0f * 2 * Runtime::deltaTime;
	*/

	// Default Scene
	mannequin->transform.rotation.y += 360.0f * 2.5f * Time::getDeltaTime();
}