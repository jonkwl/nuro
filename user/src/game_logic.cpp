#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

void awake() {
	// Import textures
	bool uploaded;
	Texture* plankTexture = new Texture("./user/assets/textures/plank.jpg", uploaded);
	Texture* obamaTexture = new Texture("./user/assets/textures/obama.jpg", uploaded);

	// Create objects
	Entity* plank = Runtime::createEntity();
	plank->position.z = 5.0f;
	plank->texture = plankTexture;

	Entity* obama = Runtime::createEntity();
	obama->position.x = 1.5f;
	obama->position.z = 5.0f;
	obama->texture = obamaTexture;
}

void update() {

}