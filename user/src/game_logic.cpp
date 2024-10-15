#include "game_logic.h"

#include <iostream>

#include "../engine/runtime/runtime.h"
#include "../engine/entity/entity.h"
#include "../engine/camera/camera.h"
#include "../engine/rendering/material/unlit/unlit_material.h"
#include "../engine/rendering/material/lit/lit_material.h"

Camera* camera = nullptr;
Entity* cube = nullptr;
Entity* floorLamp = nullptr;
Entity* mannequin = nullptr;

LitMaterial* lit;
LitMaterial* test;

int amount = 100;
std::vector<Entity*> object_batch(amount);

void awake() {
	// Set default skybox
	Runtime::setSkybox(Runtime::defaultSkybox);

	// Create camera
	camera = new Camera();
	Runtime::useCamera(camera);

	// Import textures
	Texture* testDiffuse = new Texture("./user/assets/textures/tree_bark_01_color.jpg");
	Texture* testNormal = new Texture("./user/assets/textures/tree_bark_01_normal.jpg");

	UnlitMaterial* unlit = new UnlitMaterial();

	lit = new LitMaterial();

	test = new LitMaterial();
	test->diffuseMap = testDiffuse;

	Model* cubeModel = new Model("./user/assets/models/cube.obj", { test, test });
	cube = new Entity();
	cube->model = cubeModel;
	cube->position = glm::vec3(-2.0f, 0.0f, 3.5f);

	LitMaterial* floorMaterial = new LitMaterial();
	floorMaterial->baseColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	Model* floorModel = new Model("./user/assets/models/cube.obj", { floorMaterial, floorMaterial });
	Entity* floor = new Entity();
	floor->model = floorModel;
	floor->position = glm::vec3(0.0f, -1.0f, 0.0f);
	floor->scale = glm::vec3(25.0f, 0.1f, 25.0f);

	Model* mannequinModel = new Model("./user/assets/models/mannequin.fbx", lit);
	mannequin = new Entity();
	mannequin->model = mannequinModel;
	mannequin->position = glm::vec3(1.0f, -0.9f, 5.5f);
	mannequin->rotation = glm::vec3(90.0f, 0.0f, 0.0f);
}

void update() {
	// Object rotation
	/*cube->rotation.y += 1.0f;
	mannequin->rotation.z += 1.0f;*/
}