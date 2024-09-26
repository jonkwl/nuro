#include "game_logic.h"

#include <iostream>

#include "../engine/runtime.h"
#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

void awake() {
	Entity* object = Runtime::createEntity();
	object->position.z = 5.0f;
}

void update() {

}