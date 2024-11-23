#include "game_logic.h"

#include <iostream>

#include "../src/runtime/runtime.h"
#include "../src/entity/entity.h"
#include "../src/camera/camera.h"
#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/rendering/material/lit/lit_material.h"

Entity* mannequin = nullptr;
void defaultScene() {
	// Set default skybox
	if (Runtime::defaultSkybox != nullptr) {
		Runtime::defaultSkybox->emission = 1.0f;
	}

	// Create camera
	Camera* camera = new Camera();
	Runtime::useCamera(camera);

	// Post Processing Lens Dirt
	Texture* lensDirt = new Texture("./user/assets/textures/lens_dirt.jpg", ALBEDO_MAP);
	PostProcessing::configuration.lensDirtTexture = lensDirt;
	PostProcessing::configuration.lensDirt = true;

	// Create all entities
	/*Texture* albedo = new Texture("./user/assets/textures/sci-fi/albedo.jpg", EMISSION_MAP);
	Texture* normal = new Texture("./user/assets/textures/sci-fi/normal.jpg", NORMAL_MAP);
	Texture* metallic = new Texture("./user/assets/textures/sci-fi/metallic.jpg", METALLIC_MAP);
	Texture* roughness = new Texture("./user/assets/textures/sci-fi/roughness.jpg", ROUGHNESS_MAP);
	Texture* ambient_occlusion = new Texture("./user/assets/textures/sci-fi/ambient_occlusion.jpg", AMBIENT_OCCLUSION_MAP);
	Texture* emission = new Texture("./user/assets/textures/sci-fi/emission.jpg", EMISSION_MAP);

	sphereMaterial = new LitMaterial();
	sphereMaterial->setAlbedoMap(albedo);
	sphereMaterial->setNormalMap(normal);
	sphereMaterial->setMetallicMap(metallic);
	sphereMaterial->setRoughnessMap(roughness);
	sphereMaterial->setAmbientOcclusionMap(ambient_occlusion);
	sphereMaterial->emission = true;
	sphereMaterial->setEmissionMap(emission);
	sphereMaterial->emissionIntensity = 12.5f;*/
	LitMaterial* sphereMaterial = new LitMaterial();
	sphereMaterial->roughness = 0.15f;
	Model* sphereModel = new Model("./user/assets/models/sphere.fbx");
	Entity* sphere = new Entity();
	sphere->meshRenderer->model = sphereModel;
	sphere->meshRenderer->materials.push_back(sphereMaterial);
	sphere->transform.position = glm::vec3(0.0f, 0.0f, 6.5f);

	Entity* collisionSphere = new Entity();
	collisionSphere->meshRenderer->model = sphereModel;
	collisionSphere->meshRenderer->materials.push_back(sphereMaterial);
	collisionSphere->transform.position = glm::vec3(5.0f, 0.0f, 6.5f);

	Texture* sphereAlbedo = new Texture("./user/assets/textures/mat_albedo.jpg", ALBEDO_MAP);
	Texture* sphereRoughness = new Texture("./user/assets/textures/mat_roughness.jpg", ROUGHNESS_MAP);
	Texture* sphereMetallic = new Texture("./user/assets/textures/mat_metallic.jpg", METALLIC_MAP);
	Texture* sphereNormal = new Texture("./user/assets/textures/mat_normal.jpg", NORMAL_MAP);
	LitMaterial* pbrSphereMaterial = new LitMaterial();
	pbrSphereMaterial->setAlbedoMap(sphereAlbedo);
	pbrSphereMaterial->setRoughnessMap(sphereRoughness);
	pbrSphereMaterial->setMetallicMap(sphereMetallic);
	pbrSphereMaterial->setNormalMap(sphereNormal);
	Model* pbrSphereModel = new Model("./user/assets/models/sphere.fbx");
	Entity* pbrSphere = new Entity();
	pbrSphere->meshRenderer->model = pbrSphereModel;
	pbrSphere->meshRenderer->materials.push_back(pbrSphereMaterial);
	pbrSphere->transform.position = glm::vec3(3.0f, 0.0f, 6.5f);

	Texture* plankAlbedo = new Texture("./user/assets/textures/plank.jpg", ALBEDO_MAP);
	LitMaterial* plank = new LitMaterial();
	plank->tiling = glm::vec2(2.0f, 2.0f);
	plank->setAlbedoMap(plankAlbedo);
	plank->roughness = 0.0f;
	plank->metallic = 0.0f;
	Model* cubeModel = new Model("./user/assets/models/cube.fbx");
	Entity* cube = new Entity();
	cube->meshRenderer->model = cubeModel;
	cube->meshRenderer->materials.push_back(plank);
	cube->transform.position = glm::vec3(-3.0f, 1.5f, 6.5f);

	LitMaterial* floorMaterial = new LitMaterial();
	floorMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	floorMaterial->roughness = 0.35f;
	Model* floorModel = new Model("./user/assets/models/cube.fbx");
	floorModel->castsShadow = false;
	Entity* floor = new Entity();
	floor->meshRenderer->model = floorModel;
	floor->meshRenderer->materials.push_back(floorMaterial);
	floor->transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
	floor->transform.scale = glm::vec3(150.0f, 0.1f, 150.0f);

	LitMaterial* wallMaterial = new LitMaterial();
	wallMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	wallMaterial->roughness = 0.35f;
	Model* wallModel = new Model("./user/assets/models/cube.fbx");
	wallModel->castsShadow = true;
	Entity* wall = new Entity();
	wall->meshRenderer->model = wallModel;
	wall->meshRenderer->materials.push_back(wallMaterial);
	wall->transform.position = glm::vec3(0.0f, -1.0f, 10.0f);
	wall->transform.scale = glm::vec3(7.0f, 5.0f, 0.1f);

	Texture* smearedWallAlbedo = new Texture("./user/assets/textures/Smeared Wall_BaseColor.jpg", ALBEDO_MAP);
	Texture* smearedWallRoughness = new Texture("./user/assets/textures/Smeared Wall_Roughness.jpg", ROUGHNESS_MAP);
	Texture* smearedWallNormal = new Texture("./user/assets/textures/Smeared Wall_Normal.jpg", NORMAL_MAP);
	LitMaterial* smearedWallMaterial = new LitMaterial();
	smearedWallMaterial->baseColor = glm::vec4(1.0f, 0.96f, 0.86f, 1.0f);
	smearedWallMaterial->setAlbedoMap(smearedWallAlbedo);
	smearedWallMaterial->roughness = 0.48f;
	smearedWallMaterial->setMetallicMap(smearedWallRoughness);
	smearedWallMaterial->setNormalMap(smearedWallNormal);
	smearedWallMaterial->tiling = glm::vec2(20.0f, 10.0f);
	Model* smearedWallModel = new Model("./user/assets/models/cube.fbx");
	Entity* smearedWall = new Entity();
	smearedWall->meshRenderer->model = smearedWallModel;
	smearedWall->meshRenderer->materials.push_back(smearedWallMaterial);
	smearedWall->transform.position = glm::vec3(20.0f, -1.0f, 10.0f);
	smearedWall->transform.scale = glm::vec3(10.0f, 5.0f, 0.1f);

	Texture* mannequinAlbedo = new Texture("./user/assets/textures/mannequin_albedo.jpg", ALBEDO_MAP);
	Texture* mannequinRoughness = new Texture("./user/assets/textures/mannequin_roughness.jpg", ROUGHNESS_MAP);
	Texture* mannequinMetallic = new Texture("./user/assets/textures/mannequin_metallic.jpg", METALLIC_MAP);
	LitMaterial* mannequinMaterial = new LitMaterial();
	mannequinMaterial->setAlbedoMap(mannequinAlbedo);
	mannequinMaterial->setRoughnessMap(mannequinRoughness);
	mannequinMaterial->setMetallicMap(mannequinMetallic);
	mannequinMaterial->baseColor = glm::vec4(0.5f, 0.1f, 0.1f, 1.0f);
	Model* mannequinModel = new Model("./user/assets/models/mannequin.fbx");
	mannequin = new Entity();
	mannequin = new Entity();
	mannequin->meshRenderer->model = mannequinModel;
	mannequin->meshRenderer->materials.push_back(mannequinMaterial);
	mannequin->transform.position = glm::vec3(14.0f, -0.9f, 6.4f);
	mannequin->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	mannequin->transform.scale = glm::vec3(1.4f);

	LitMaterial* whiteMaterial = new LitMaterial();
	whiteMaterial->baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	whiteMaterial->roughness = 0.15f;
	Model* shadowedSphereModel = new Model("./user/assets/models/sphere.fbx");
	Entity* shadowedSphere = new Entity();
	shadowedSphere->meshRenderer->model = shadowedSphereModel;
	shadowedSphere->meshRenderer->materials.push_back(whiteMaterial);
	shadowedSphere->transform.position = glm::vec3(12.5f, 0.4f, 8.0f);

	LitMaterial* neon = new LitMaterial();
	neon->emission = true;
	neon->emissionColor = glm::vec4(0.5f, 0.2f, 1.0f, 1.0f);
	neon->emissionIntensity = 0.8f;
	Model* neonModel = new Model("./user/assets/models/pacman_neon_lights.fbx");
	Entity* neonLight = new Entity();
	neonLight->transform.position = glm::vec3(14.5f, 2.5f, 9.5f);
	neonLight->transform.scale = glm::vec3(5.0f, 5.0f, 5.0f);
	neonLight->meshRenderer->model = neonModel;
	neonLight->meshRenderer->materials = { neon, neon, neon, neon, neon, neon, neon };

	for (int i = 0; i < 40; i++) {
		Entity* x = new Entity();
		x = new Entity();
		x->meshRenderer->model = mannequinModel;
		x->meshRenderer->materials.push_back(mannequinMaterial);
		x->transform.position = glm::vec3(14.0f, -0.9f, 8.5f);
		x->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
		x->transform.scale = glm::vec3(1.4f);

		x->transform.position.x += 3.0f * (i + 1);
	}
}

std::vector<Entity*> cubes = std::vector<Entity*>();
void performanceScene() {
	int gridX = 5;
	int gridZ = 5;
	float offset = 3.0f;

	Model* cube = new Model("./user/assets/models/cube.fbx");
	
	LitMaterial* material = new LitMaterial();
	material->baseColor = glm::vec4(1.0f);
	material->roughness = 0.1f;
	material->metallic = 0.0f;

	Runtime::directionalIntensity = 3.0f;
	PostProcessing::configuration.bloomIntensity = 0.25f;

	glm::vec2 halfSize = glm::vec2(gridX * offset, gridZ * offset);

	for (int x = 1; x < gridX + 1; x++) {
		for (int z = 1; z < gridZ + 1; z++) {
			Entity* entity = new Entity();
			entity = new Entity();
			entity->meshRenderer->model = cube;
			entity->meshRenderer->materials.push_back(material);
			entity->transform.position = glm::vec3(14.0f, -0.9f, 8.5f);
			entity->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
			entity->transform.scale = glm::vec3(1.0f);
			entity->transform.position.x += offset * x - halfSize.x;
			entity->transform.position.y = -3.5f;
			entity->transform.position.z += offset * z - halfSize.y;
			cubes.push_back(entity);
		}
	}

	Entity* entity = new Entity();
	entity = new Entity();
	entity->meshRenderer->model = cube;
	entity->meshRenderer->materials.push_back(material);
	entity->transform.position = glm::vec3(0.0f, 1.5f, 4.0f);
	entity->transform.scale.x = 2.0f;
	cubes.push_back(entity);
}

void awake() {
	defaultScene();
}

void update() {
	/*float range = 1.0f;
	for (int i = 0; i < cubes.size(); i++) {
		cubes.at(i)->transform.position.y = range * std::sin(Runtime::time * (float)i);
	}
	cubes.at(cubes.size() - 1)->transform.rotation.z += 360.0f * 2 * Runtime::deltaTime;*/
	mannequin->transform.rotation.y += 360.0f * Runtime::deltaTime;
}