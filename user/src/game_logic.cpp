#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

Camera* camera = nullptr;
Entity* plank = nullptr;
Entity* dirt = nullptr;

void awake() {
	// Import textures
	bool uploaded;
	Texture* plankTexture = new Texture("./user/assets/textures/plank.jpg", uploaded);
	Texture* dirtTexture = new Texture("./user/assets/textures/dirt.jpg", uploaded);

	// Use camera
	camera = new Camera();
	Runtime::useCamera(camera); // Be aware: Inspector mode is activated, so this camera won't be rendering

	// Create objects
	plank = Runtime::createEntity();
	plank->position.z = 5.0f;
	plank->texture = plankTexture;

	dirt = Runtime::createEntity();
	dirt->texture = dirtTexture;
}

void update() {
	dirt->position = glm::vec3(2.0f, (sin(Runtime::time * 2) + 1) / 2, 5.0f);
	dirt->rotation += glm::vec3(50.0f * Runtime::delta_time, 30.0f * Runtime::delta_time, 15.0f * Runtime::delta_time);
}