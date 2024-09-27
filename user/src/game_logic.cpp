#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"
#include "../engine/rendering/material/unlit_material.h"

Camera* camera = nullptr;
Entity* floorLamp = nullptr;
Entity* mannequin = nullptr;

int amount = 100;
std::vector<Entity*> object_batch(amount);

void awake() {
	// Create camera
	camera = new Camera();
	Runtime::useCamera(camera);

	// Import textures
	bool success;
	Texture* exampleTexture = new Texture("./user/assets/textures/plank.jpg", success);

	UnlitMaterial* unlit = new UnlitMaterial();

	Model* floorLampModel = new Model("./user/assets/models/floor_lamp.fbx", unlit);
	Model* mannequinModel = new Model("./user/assets/models/mannequin.fbx", unlit);

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
	mannequin->rotation.z += 0.5f;
}