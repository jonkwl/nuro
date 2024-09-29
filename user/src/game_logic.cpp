#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"
#include "../engine/rendering/material/unlit_material.h"
#include "../engine/rendering/material/lit_material.h"
#include "../engine/rendering/material/rainbow_material.h"

Camera* camera = nullptr;
Entity* cube = nullptr;
Entity* floorLamp = nullptr;
Entity* mannequin = nullptr;

int amount = 100;
std::vector<Entity*> object_batch(amount);

void awake() {
	// Create camera
	camera = new Camera();
	Runtime::useCamera(camera);

	// Import textures
	Texture* dirtTexture = new Texture("./user/assets/textures/dirt.jpg");

	UnlitMaterial* unlit = new UnlitMaterial(Runtime::defaultTexture);
	LitMaterial* lit = new LitMaterial(Runtime::defaultTexture);
	RainbowMaterial* rainbow = new RainbowMaterial();

	LitMaterial* dirt = new LitMaterial(dirtTexture);

	LitMaterial* lightGray = new LitMaterial(Runtime::defaultTexture);
	lightGray->baseColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	LitMaterial* darkGray = new LitMaterial(Runtime::defaultTexture);
	darkGray->baseColor = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
	LitMaterial* white = new LitMaterial(Runtime::defaultTexture);
	white->baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	LitMaterial* black = new LitMaterial(Runtime::defaultTexture);
	black->baseColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	Model* cubeModel = new Model("./user/assets/models/cube.obj", { dirt, dirt });
	Model* floorLampModel = new Model("./user/assets/models/floor_lamp.fbx", { lightGray, black, darkGray, white });
	Model* mannequinModel = new Model("./user/assets/models/mannequin.fbx", lit);

	cube = Runtime::createEntity();
	cube->model = cubeModel;
	cube->position = glm::vec3(0.0f, 0.0f, 0.0f);

	floorLamp = Runtime::createEntity();
	floorLamp->model = floorLampModel;
	floorLamp->position = glm::vec3(-1.0f, 0.0f, 5.0f);
	floorLamp->rotation = glm::vec3(90.0f, 0.0f, 90.0f);
	floorLamp->scale = glm::vec3(1.5f, 1.5f, 1.5f);

	mannequin = Runtime::createEntity();
	mannequin->model = mannequinModel;
	mannequin->position = glm::vec3(1.0f, 0.0f, 5.0f);
	mannequin->rotation = glm::vec3(90.0f, 0.0f, 0.0f);
}

void update() {
	cube->rotation += glm::vec3(0.5, 0.5, 0.0f);
}