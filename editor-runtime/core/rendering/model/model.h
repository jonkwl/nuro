#pragma once

#include <cstdint>
#include <string>
#include <glm.hpp>
#include <assimp/scene.h>

#include "../core/rendering/model/mesh.h"

class Model
{
public:
	// Loads and returns the model from provided path
	static Model* load(std::string path);

	// Destroys the model and all of its meshes
	~Model();

	struct Metrics
	{
		// Total number of meshes in model
		uint32_t nMeshes = 0;

		// Total number of polygon faces in model
		uint32_t nFaces = 0;

		// Total number of vertices in model
		uint32_t nVertices = 0;

		// Total number of materials in model
		uint32_t nMaterials = 0;

		// Minimum vertex position (bounding box)
		glm::vec3 minPoint = glm::vec3(FLT_MAX);

		// Maximum vertex position (bounding box)
		glm::vec3 maxPoint = glm::vec3(-FLT_MAX);

		// Center of model (midpoint of min/max points)
		glm::vec3 origin = glm::vec3(0.0f);

		// Geometric center (average of all vertices)
		glm::vec3 centroid = glm::vec3(0.0f);

		// Maximum distance of a vertice from object space center
		float furthest = 0.0f;
	};

	// Returns models metrics
	Metrics getMetrics() const;

	// Returns models mesh at given index
	const Mesh* getMesh(uint32_t index);

	// Returns all meshes from model
	const std::vector<Mesh>& getMeshes();

private:
	explicit Model(std::string path);

	void resolveModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Mesh> meshes;

	std::vector<aiMaterial*> modelMaterials;

	// Model metrics
	Metrics metrics;
	void addMeshToMetrics(std::vector<Mesh::VertexData> vertices); // Called for each of the models meshes
	void finalizeMetrics(); // Called after all meshes were added to metrics
};