#pragma once

#include <cstdint>
#include <string>
#include <glm.hpp>
#include <assimp/scene.h>

#include "../core/rendering/model/mesh.h"

#include "../core/resource/resource.h"

class Model : public Resource
{
public:
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

public:
	Model();

	// Sets the path of the models source
	void setSource(std::string path);

	// Returns models metrics
	Metrics getMetrics() const;

	// Returns models mesh at given index
	const Mesh* getMesh(uint32_t index);

	// Returns all meshes from model
	const std::vector<Mesh>& getMeshes();

	std::string sourcePath() override;

protected:
	void loadData() override;
	void releaseData() override;
	void dispatchGPU() override;

private:
	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct MeshData {
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;

		explicit MeshData(std::vector<VertexData> vertices, std::vector<uint32_t> indices, uint32_t materialIndex) : vertices(vertices), indices(indices), materialIndex(materialIndex) {};
	};

private:
	//
	// MODEL CREATION
	//

	void processNode(aiNode* node, const aiScene* scene);
	MeshData processMesh(aiMesh* mesh, const aiScene* scene);

	void dispatchMesh(MeshData meshData);

	//
	// MODEL DATA
	//

	// Path of models source
	std::string path;

	// Intermediate temporary representation of mesh data
	std::vector<MeshData> data;

	// Final dispatched meshes
	std::vector<Mesh> meshes;

	//
	// MODEL METRICS
	//

	// Models metrics
	Metrics metrics;

	// Adds a mesh to the metrics using its vertices
	void addMeshToMetrics(const std::vector<VertexData>& vertices);
	
	// Finalizes the metrics after all meshes have been added
	void finalizeMetrics();
};