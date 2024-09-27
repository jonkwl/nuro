#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"
#include "../engine/rendering/material/unlit_material.h"

Camera* camera = nullptr;
Entity* plank = nullptr;
Entity* dirt = nullptr;

int amount = 100;
std::vector<Entity*> object_batch(amount);

void awake() {
	// Create camera
	camera = new Camera();
	Runtime::useCamera(camera);

	// Import textures
	bool success;
	Texture* plankTexture = new Texture("./user/assets/textures/plank.jpg", success);
	Texture* dirtTexture = new Texture("./user/assets/textures/dirt.jpg", success);

	UnlitMaterial* plank = new UnlitMaterial();
	plank->texture = plankTexture;

	Model* cubeModel = new Model("./user/assets/models/air_hockey.fbx");

	/* for (int i = 0; i < amount; i++) {
		object_batch[i] = Runtime::createEntity();
		object_batch[i]->material = plank;
		object_batch[i]->position = glm::vec3((i % 10) * 2.0f, 0, (i / 10) * 2.0f);
	} */

	Entity* meshEntity = Runtime::createEntity();
	meshEntity->material = plank;
	meshEntity->model = cubeModel;
	meshEntity->position.z = 5;
}

void update() {
	/*
	for (int i = 0; i < amount; i++) {
		object_batch[i]->position = glm::vec3(object_batch[i]->position.x, (sin(Runtime::time * 2) + 1) / 2, object_batch[i]->position.z);
		object_batch[i]->rotation += glm::vec3(50.0f * Runtime::deltaTime, 30.0f * Runtime::deltaTime, 15.0f * Runtime::deltaTime);
	}
	*/
}