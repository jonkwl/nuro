#include "game_logic.h"

#include "../core/engine.h";
#include "../src/runtime/runtime.h"

void defaultScene() {
	LitMaterial* sphereMaterial = new LitMaterial();
	sphereMaterial->roughness = 0.15f;
	Model* sphereModel = Model::load("./src/example/assets/models/sphere.fbx");
	OldEntity* sphere = new OldEntity();
	Runtime::entityStack.push_back(sphere);
	sphere->meshRenderer.model = sphereModel;
	sphere->meshRenderer.materials.push_back(sphereMaterial);
	sphere->transform.position = glm::vec3(0.0f, 0.6f, 6.5f);
}

void awake() {
	defaultScene();
}

void update() {
}