#include "game_logic.h"

#include "../core/engine.h";
#include "../src/runtime/runtime.h"

entity camera;

entity cube1;
physx::PxRigidDynamic* cube1Rb = nullptr;

entity cube2;
physx::PxRigidDynamic* cube2Rb = nullptr;

entity cube3;
physx::PxRigidDynamic* cube3Rb = nullptr;

entity cube4;
physx::PxRigidDynamic* cube4Rb = nullptr;

entity cube5;
physx::PxRigidDynamic* cube5Rb = nullptr;

// tmp
void createPhysicsEntity(glm::vec3 position, glm::vec3 rotation, entity& e, physx::PxRigidDynamic*& rb, Mesh& mesh) {
	e = ECS::createEntity();

	TransformComponent& cubeTransform = ECS::addComponent<TransformComponent>(e);
	ECS::addComponent<MeshRendererComponent>(e, mesh);
	cubeTransform.position = position;

	Transform::rotateX(cubeTransform, rotation.x);
	Transform::rotateY(cubeTransform, rotation.y);
	Transform::rotateZ(cubeTransform, rotation.z);

	rb = Runtime::getPhysicsController().createDynamicBox(
		physx::PxVec3(cubeTransform.position.x, cubeTransform.position.y, cubeTransform.position.z),
		physx::PxQuat(cubeTransform.rotation.x, cubeTransform.rotation.y, cubeTransform.rotation.z, cubeTransform.rotation.w),
		physx::PxVec3(cubeTransform.scale.x, cubeTransform.scale.y, cubeTransform.scale.z));
}

void updatePhysicsEntity(entity e, physx::PxRigidDynamic* rb) {
	TransformComponent& transform = ECS::getComponent<TransformComponent>(e);
	physx::PxTransform pose = rb->getGlobalPose();
	physx::PxVec3 position = pose.p;
	physx::PxQuat rotation = pose.q;
	transform.position = glm::vec3(position.x, position.y, position.z);
	transform.rotation = glm::quat(rotation.x, rotation.y, rotation.z, rotation.w);
}

void setup() {

	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	camera = ECS::createEntity();
	ECS::addComponent<TransformComponent>(camera);
	ECS::addComponent<CameraComponent>(camera);

	createPhysicsEntity(glm::vec3(0.0f, 8.0f, 16.0f), glm::vec3(0.0f), cube1, cube1Rb, cubeMesh);
	createPhysicsEntity(glm::vec3(0.2f, 5.0f, 16.0f), glm::vec3(0.0f), cube2, cube2Rb, cubeMesh);
	createPhysicsEntity(glm::vec3(-0.8f, 3.0f, 18.0f), glm::vec3(0.0f), cube3, cube3Rb, cubeMesh);
	createPhysicsEntity (glm::vec3(3.0f, 0.0f, 14.0f), glm::vec3(0.0f), cube4, cube4Rb, cubeMesh);
	createPhysicsEntity(glm::vec3(4.0f, 1.5f, 16.0f), glm::vec3(0.0f), cube5, cube5Rb, cubeMesh);

	/*entity ground = ECS::createEntity();
	TransformComponent& groundTransform = ECS::addComponent<TransformComponent>(ground);
	ECS::addComponent<MeshRendererComponent>(ground, cubeMesh);
	groundTransform.position = glm::vec3(0.0f, -8.45f, 16.0f);
	groundTransform.scale = glm::vec3(18.0f, 0.1f, 18.0f);*/
	Runtime::getPhysicsController().createStaticPlane(physx::PxVec3(0.0f, 1.0, 0.0f), 8.0f);

}

void awake() {
	Log::printProcessInfo("Game Start");
}

void quit()
{
	Log::printProcessInfo("Game End");
}

void update() {

	updatePhysicsEntity(cube1, cube1Rb);
	updatePhysicsEntity(cube2, cube2Rb);
	updatePhysicsEntity(cube3, cube3Rb);
	updatePhysicsEntity(cube4, cube4Rb);
	updatePhysicsEntity(cube5, cube5Rb);

}