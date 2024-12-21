#include "game_logic.h"

#include "../core/engine.h";
#include "../src/runtime/runtime.h"

Entity camera;
Entity sphere;
Entity cube;

void awake() {

	camera = Entity::create();
	camera.addComponent<TransformComponent>();
	camera.addComponent<CameraComponent>();

	sphere = Entity::create();
	TransformComponent& sphereTransform = sphere.addComponent<TransformComponent>();
	MeshRendererComponent& sphereRenderer = sphere.addComponent<MeshRendererComponent>(Model::load("../resources/primitives/sphere.fbx")->meshes[0]);
	sphereTransform.position.z = 8.0f;

	cube = Entity::create();
	TransformComponent& cubeTransform = cube.addComponent<TransformComponent>();
	MeshRendererComponent& cubeRenderer = cube.addComponent<MeshRendererComponent>(Model::load("../resources/primitives/cube.fbx")->meshes[0]);
	cubeTransform.position.z = 10.0f;
	cubeTransform.scale = glm::vec3(8.0f, 8.0f, 0.1f);

}

void update() {
}