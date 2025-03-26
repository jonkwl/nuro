#pragma once

#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <unordered_map>

#include <memory/resource.h>
#include <rendering/model/mesh.h>

class aiScene;
class aiNode;
class aiMesh;

class Model : public Resource
{
public:
	Model();
	~Model() override;

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

	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		VertexData() : position(), normal(), uv(), tangent(), bitangent() {};

		explicit VertexData(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec3 tangent, glm::vec3 bitangent) : position(position),
			normal(normal),
			uv(uv),
			tangent(tangent),
			bitangent(bitangent)
		{
		};
	};

	struct MeshData {
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;

		explicit MeshData(std::vector<VertexData>&& vertices, std::vector<uint32_t>&& indices, uint32_t materialIndex) :
			vertices(std::move(vertices)),
			indices(std::move(indices)),
			materialIndex(materialIndex)
		{
		};
	};

	// Default pipe for creating model
	ResourcePipe create() {
		return std::move(pipe()
			>> BIND_TASK(Model, loadIoData)
			>> BIND_TASK_WITH_FLAGS(Model, uploadBuffers, TaskFlags::UseContextThread));
	}

	// Sets the path of the models source
	void setSource(std::string path);

	// Returns models mesh at given index or creates an empty mesh at that index
	const Mesh* queryMesh(uint32_t index);

	// Returns amount of loaded and dispatched meshes
	uint32_t nLoadedMeshes() const;

	// Returns models metrics
	Metrics getMetrics() const;

public:
	// Creates a static mesh with the given vertices and indices
	static Mesh* createStaticMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices);

private:
	//
	// MODEL CREATION
	//

	void processNode(aiNode* node, const aiScene* scene);
	MeshData processMesh(aiMesh* mesh, const aiScene* scene);

	bool loadIoData();
	void freeIoData();
	bool uploadBuffers();
	void deleteBuffers();

	//
	// MODEL DATA
	//

	// Path of models source
	std::string path;

	// Intermediate temporary representation of mesh data
	std::vector<MeshData> meshData;

	// Final dispatched meshes
	std::unordered_map<uint32_t, Mesh> meshes;

	//
	// MODEL METRICS
	//

	// Models metrics
	Metrics metrics;

	// Adds a mesh to the metrics using its vertices
	void addMeshToMetrics(const std::vector<VertexData>& vertices, uint32_t nFaces);
	
	// Finalizes the metrics after all meshes have been added
	void finalizeMetrics();
};