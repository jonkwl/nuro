#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

Camera* camera = nullptr;
Entity* plank = nullptr;
Entity* dirt = nullptr;

int amount = 100;
std::vector<Entity*> object_batch(amount);

void awake() {
	// Runtime settings
	Runtime::inspectorMode = true;
	Runtime::showDiagnostics = true;

	// Import textures
	bool uploaded;
	Texture* plankTexture = new Texture("./user/assets/textures/plank.jpg", uploaded);
	Texture* dirtTexture = new Texture("./user/assets/textures/dirt.jpg", uploaded);

	// Use camera
	camera = new Camera();
	Runtime::useCamera(camera); // Be aware: Inspector mode is activated, so this camera won't be rendering

	// Create objects
	// plank = Runtime::createEntity();
	// plank->position.z = 5.0f;
	// plank->texture = plankTexture;

	// dirt = Runtime::createEntity();
	// dirt->texture = dirtTexture;

	for (int i = 0; i < amount; i++) {
		object_batch[i] = Runtime::createEntity();
		object_batch[i]->texture = plankTexture;
		object_batch[i]->position = glm::vec3((i % 10) * 2.0f, 0, (i / 10) * 2.0f);
	}
}

void update() {
	for (int i = 0; i < amount; i++) {
		object_batch[i]->position = glm::vec3(object_batch[i]->position.x, (sin(Runtime::time * 2) + 1) / 2, object_batch[i]->position.z);
		object_batch[i]->rotation += glm::vec3(50.0f * Runtime::deltaTime, 30.0f * Runtime::deltaTime, 15.0f * Runtime::deltaTime);
	}
}