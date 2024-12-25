#include "game_logic.h"

#include "../core/engine.h";

Mesh* getQuadMesh() {
    // Positions for quad vertices
    glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 pos4(1.0f, 1.0f, 0.0f);

    // Texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);

    // Normal vector (all faces pointing in the +Z direction)
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // Calculate tangent/bitangent vectors for both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;

    // Triangle 1
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // Triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);

    // Vertex data for the quad (6 vertices, 2 triangles)
    std::vector<Mesh::VertexData> vertices = {
        // Triangle 1
        {pos1, nm, uv1, tangent1, bitangent1},
        {pos2, nm, uv2, tangent1, bitangent1},
        {pos3, nm, uv3, tangent1, bitangent1},

        // Triangle 2
        {pos1, nm, uv1, tangent2, bitangent2},
        {pos3, nm, uv3, tangent2, bitangent2},
        {pos4, nm, uv4, tangent2, bitangent2}
    };

    // Indices for the triangles (two triangles, each with 3 vertices)
    std::vector<uint32_t> indices = {
        0, 1, 2,  // Triangle 1
        3, 4, 5   // Triangle 2
    };

    // Return a new Mesh object initialized with the vertices and indices
    return new Mesh(vertices, indices);
}

Entity kinematicEntity;

void setup() {

	// Models
	Model* cubeModel = Model::load("../resources/primitives/cube.fbx");
	Model* sphereModel = Model::load("../resources/primitives/sphere.fbx");

	// Meshes
    Mesh* planeMesh = getQuadMesh();
	Mesh& cubeMesh = cubeModel->getMesh(0);
	Mesh& sphereMesh = sphereModel->getMesh(0);

	// Materials
	LitMaterial* standardMaterial = new LitMaterial();
	standardMaterial->baseColor = glm::vec4(glm::vec3(0.25f), 1.0f);
	standardMaterial->roughness = 0.4f;

	// Camera
	EntityContainer camera(ECS::createEntity());
	camera.add<CameraComponent>();

	// Ground
	EntityContainer ground(ECS::createEntity());
	ground.transform.position = glm::vec3(0.0f, -10.1f, 35.0f);
	ground.transform.scale = glm::vec3(140.0f, 0.1f, 140.0f);
	ground.add<MeshRendererComponent>(*planeMesh, standardMaterial);

	// Concrete
	EntityContainer concreteBlock(ECS::createEntity());
	concreteBlock.transform.position = glm::vec3(0.0f, 0.0f, 5.0f);
	concreteBlock.add<MeshRendererComponent>(cubeMesh, standardMaterial);
	concreteBlock.add<BoxColliderComponent>();
	RigidbodyComponent& concreteRb = concreteBlock.add<RigidbodyComponent>();
	Rigidbody::setGravity(concreteRb, false);  

	// Cube batch
	int objectAmount = 140;
	for (int x = 0; x < std::sqrt(objectAmount); x++) {
		for (int y = 0; y < std::sqrt(objectAmount); y++) {
			EntityContainer e(ECS::createEntity());
			e.transform.position = glm::vec3(x * 2.5f - 8.0f, y * 2.5f - 8.0f, 35.0f);
			MeshRendererComponent& r = e.add<MeshRendererComponent>(cubeMesh, standardMaterial);
			e.add<BoxColliderComponent>();
			RigidbodyComponent& rb = e.add<RigidbodyComponent>();
		}
	}

	// Kinematic sphere
	EntityContainer kinematic(ECS::createEntity());
	kinematic.transform.position = glm::vec3(12.0f, 6.0f, 100.0f);
	kinematic.transform.scale = glm::vec3(6.0f);
	kinematic.add<MeshRendererComponent>(sphereMesh, standardMaterial);
	kinematic.add<SphereColliderComponent>();
	RigidbodyComponent& kinematicRb = kinematic.add<RigidbodyComponent>();
	Rigidbody::setCollisionDetection(kinematicRb, RigidbodyComponent::CollisionDetection::CONTINUOUS);
	Rigidbody::setKinematic(kinematicRb, true);
	kinematicEntity = kinematic.root;

}

void awake() {
	Log::printProcessInfo("Game Start");
}

void quit()
{
	Log::printProcessInfo("Game End");
}

void update() {

	EntityContainer kinematic = EntityContainer(kinematicEntity);
	kinematic.transform.position += glm::vec3(0.0f, 0.0f, -32.0f * Time::deltaf());

}