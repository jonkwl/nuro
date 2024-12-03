#pragma once

#include <string>
#include <glm.hpp>
#include <assimp/scene.h>

#include "../core/rendering/model/mesh.h"

class Model
{
public:
	static Model* load(std::string path); // Creates a new model on the heap using the corresponding constructor

	struct Metrics
	{
		unsigned int nMeshes = 0;	 // Total number of meshes in model
		unsigned int nFaces = 0;	 // Total number of polygon faces in model
		unsigned int nVertices = 0;	 // Total number of vertices in model
		unsigned int nMaterials = 0; // Total number of materials in model

		glm::vec3 minPoint = glm::vec3(FLT_MAX); // Minimum vertex position (bounding box)
		glm::vec3 maxPoint = glm::vec3(-FLT_MAX); // Maximum vertex position (bounding box)
		glm::vec3 origin = glm::vec3(0.0f);	  // Center of model (midpoint of min/max points)
		glm::vec3 centroid = glm::vec3(0.0f); // Geometric center (average of all vertices)
		float furthest = 0.0f;				  // Maximum distance of a vertice from object space center
	};

	bool castsShadow;
	std::vector<Mesh> meshes;
	Metrics getMetrics() const; // Returns the model metrics

private:
	explicit Model(std::string path);

	void resolveModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<aiMaterial*> modelMaterials;

	// Model metrics
	Metrics metrics;
	void addMeshToMetrics(std::vector<Mesh::VertexData> vertices); // Called for each of the models meshes
	void finalizeMetrics(); // Called after all meshes were added to metrics
};