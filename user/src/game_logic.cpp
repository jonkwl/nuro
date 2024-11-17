#include "game_logic.h"

#include <iostream>

#include "../src/runtime/runtime.h"
#include "../src/entity/entity.h"
#include "../src/camera/camera.h"
#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/rendering/material/lit/lit_material.h"

Camera* camera = nullptr;
Entity* cube = nullptr;
Entity* mannequin = nullptr;

LitMaterial* sphereMaterial = nullptr;

void awake() {
	// Set default skybox
	Runtime::setSkybox(Runtime::defaultSkybox);
	if (Runtime::defaultSkybox != nullptr) {
		Runtime::defaultSkybox->emission = 1.0f;
	}

	// Create camera
	camera = new Camera();
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
	sphereMaterial->setEmissionMap(emission);
	sphereMaterial->emissionIntensity = 12.5f;
	Model* sphereModel = new Model("./user/assets/models/sphere.fbx", { sphereMaterial });
	Entity* sphere = new Entity();
	sphere->meshRenderer->model = sphereModel;
	sphere->transform.position = glm::vec3(0.0f, 0.0f, 6.5f);*/

	Texture* sphereAlbedo = new Texture("./user/assets/textures/mat_albedo.jpg", ALBEDO_MAP);
	Texture* sphereRoughness = new Texture("./user/assets/textures/mat_roughness.jpg", ROUGHNESS_MAP);
	Texture* sphereMetallic = new Texture("./user/assets/textures/mat_metallic.jpg", METALLIC_MAP);
	Texture* sphereNormal = new Texture("./user/assets/textures/mat_normal.jpg", NORMAL_MAP);
	LitMaterial* pbrSphereMaterial = new LitMaterial();
	pbrSphereMaterial->setAlbedoMap(sphereAlbedo);
	pbrSphereMaterial->setRoughnessMap(sphereRoughness);
	pbrSphereMaterial->setMetallicMap(sphereMetallic);
	pbrSphereMaterial->setNormalMap(sphereNormal);
	Model* pbrSphereModel = new Model("./user/assets/models/sphere.fbx", { pbrSphereMaterial });
	Entity* pbrSphere = new Entity();
	pbrSphere->meshRenderer->model = pbrSphereModel;
	pbrSphere->transform.position = glm::vec3(3.0f, 0.0f, 6.5f);

	Texture* plankAlbedo = new Texture("./user/assets/textures/plank.jpg", ALBEDO_MAP);
	LitMaterial* plank = new LitMaterial();
	plank->tiling = glm::vec2(2.0f,  2.0f);
	plank->setAlbedoMap(plankAlbedo);
	plank->roughness = 0.0f;
	plank->metallic = 0.0f;
	Model* cubeModel = new Model("./user/assets/models/cube.fbx", plank);
	cube = new Entity();
	cube->meshRenderer->model = cubeModel;
	cube->transform.position = glm::vec3(-3.0f, 1.5f, 6.5f);

	LitMaterial* floorMaterial = new LitMaterial();
	floorMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	floorMaterial->roughness = 0.35f;
	Model* floorModel = new Model("./user/assets/models/cube.fbx", floorMaterial);
	floorModel->castsShadow = false;
	Entity* floor = new Entity();
	floor->meshRenderer->model = floorModel;
	floor->transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
	floor->transform.scale = glm::vec3(150.0f, 0.1f, 150.0f);

	LitMaterial* wallMaterial = new LitMaterial();
	wallMaterial->baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	wallMaterial->roughness = 0.35f;
	Model* wallModel = new Model("./user/assets/models/cube.fbx", wallMaterial);
	wallModel->castsShadow = true;
	Entity* wall = new Entity();
	wall->meshRenderer->model = wallModel;
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
	Model* smearedWallModel = new Model("./user/assets/models/cube.fbx", smearedWallMaterial);
	Entity* smearedWall = new Entity();
	smearedWall->meshRenderer->model = smearedWallModel;
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
	Model* mannequinModel = new Model("./user/assets/models/mannequin.fbx", mannequinMaterial);
	mannequin = new Entity();
	mannequin = new Entity();
	mannequin->meshRenderer->model = mannequinModel;
	mannequin->transform.position = glm::vec3(14.0f, -0.9f, 6.0f);
	mannequin->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	mannequin->transform.scale = glm::vec3(1.4f);

	LitMaterial* whiteMaterial = new LitMaterial();
	whiteMaterial->baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	whiteMaterial->roughness = 0.15f;
	Model* shadowedSphereModel = new Model("./user/assets/models/sphere.fbx", { whiteMaterial });
	Entity* shadowedSphere = new Entity();
	shadowedSphere->meshRenderer->model = shadowedSphereModel;
	shadowedSphere->transform.position = glm::vec3(12.5f, 0.4f, 8.0f);

	for (int i = 0; i < 40; i++) {
		Entity* x = new Entity();
		x = new Entity();
		x->meshRenderer->model = mannequinModel;
		x->meshRenderer->overwriteMaterials = true;
		x->meshRenderer->materials.push_back(pbrSphereMaterial);
		x->transform.position = glm::vec3(14.0f, 0.0f, 8.5f);
		x->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
		x->transform.scale = glm::vec3(1.4f);

		x->transform.position.x += 5.0f * (i + 1);
	}
}

void update() {
	cube->transform.rotation.y += 20.0f * Runtime::deltaTime;
	mannequin->transform.rotation.z += 30.0f * Runtime::deltaTime;
}